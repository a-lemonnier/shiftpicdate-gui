#include "shiftpicdate_win32.h"


std::string spdFunc::getExifDate(const std::string &sFilename) {
    std::string Res;
        
    if (  fs::exists(fs::path(sFilename)) && test_ext(sFilename)) {
        std::wstring wsFilename(sFilename.begin(), sFilename.end());

        CoInitialize( nullptr );
        
        CComPtr<IWICImagingFactory> pFactory;
        CComPtr<IWICBitmapDecoder> pDecoder;
        CComPtr<IWICBitmapFrameDecode> pFrame;
        CComPtr<IWICMetadataQueryReader> pMetadataReader;
        
        CoCreateInstance( CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&pFactory );
        
        auto hr=pFactory->CreateDecoderFromFilename(wsFilename.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pDecoder );
        
        if (SUCCEEDED(hr)) {
            pDecoder->GetFrame( 0, &pFrame );
            
            pFrame->GetMetadataQueryReader( &pMetadataReader );
            
            PROPVARIANT value;
            PropVariantInit( &value );
            
            pMetadataReader->GetMetadataByName( L"/app1/ifd/exif/{ushort=36867}", &value );
            if (value.vt == VT_LPSTR)
                Res=std::string(value.pszVal);
            PropVariantClear( &value );

            pMetadataReader.Release();
            pFrame.Release();
            pDecoder.Release();
            pFactory.Release();
        }
        else std::cerr << "Cannot create decoder.\n";
        CoUninitialize( );   
    }
    return Res;
}


bool spdFunc::setExifDate(const std::string &sFilename, long long Diff, bool bIsDST) {
    bool bStatus=false;
    if (spdFunc::test_ext(sFilename)) {
        
        std::string sTimestamp=spdFunc::shiftTimestamp(getExifDate(sFilename), Diff, bIsDST);
        
        std::string sFilename_new=sFilename+"_new";
        std::wstring wsFilename(sFilename.begin(), sFilename.end());
        std::wstring wsFilename_new(sFilename_new.begin(), sFilename_new.end());
                
        // Initialize COM.
        HRESULT hr =CoInitialize( nullptr );
        
        IWICImagingFactory *piFactory = NULL;
        IWICBitmapDecoder *piDecoder = NULL;
        
        // Create the COM imaging factory.
        if (SUCCEEDED(hr))
            hr = CoCreateInstance(CLSID_WICImagingFactory,
                                  NULL, CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&piFactory));
            
        // Create the decoder.
        if (SUCCEEDED(hr))
            hr = piFactory->CreateDecoderFromFilename(wsFilename.c_str(), NULL, GENERIC_READ,
                                                          WICDecodeMetadataCacheOnDemand, //For JPEG lossless decoding/encoding.
                                                          &piDecoder);
                
        // Variables used for encoding.
        IWICStream *piFileStream = NULL;
        IWICBitmapEncoder *piEncoder = NULL;
        IWICMetadataBlockWriter *piBlockWriter = NULL;
        IWICMetadataBlockReader *piBlockReader = NULL;

        WICPixelFormatGUID pixelFormat = { 0, 0, 0, { 0 } };
        UINT count = 0;
        double dpiX, dpiY = 0.0;
        UINT width, height = 0;

        // Create a file stream.
        if (SUCCEEDED(hr)) hr = piFactory->CreateStream(&piFileStream);

        // Initialize our new file stream.
        if (SUCCEEDED(hr)) hr = piFileStream->InitializeFromFilename(wsFilename_new.c_str(), GENERIC_WRITE);

        // Create the encoder.
        if (SUCCEEDED(hr)) hr = piFactory->CreateEncoder(GUID_ContainerFormatJpeg, NULL, &piEncoder);

        // Initialize the encoder
        if (SUCCEEDED(hr)) hr = piEncoder->Initialize(piFileStream,WICBitmapEncoderNoCache);

        if (SUCCEEDED(hr)) hr = piDecoder->GetFrameCount(&count);

        if (SUCCEEDED(hr)) { // Process each frame of the image.
            for (UINT i=0; i<count && SUCCEEDED(hr); i++) { // Frame variables.

                IWICBitmapFrameDecode *piFrameDecode = NULL;
                IWICBitmapFrameEncode *piFrameEncode = NULL;
                IWICMetadataQueryReader *piFrameQReader = NULL;
                IWICMetadataQueryWriter *piFrameQWriter = NULL;

                // Get and create the image frame.
                if (SUCCEEDED(hr)) hr = piDecoder->GetFrame(i, &piFrameDecode);

                if (SUCCEEDED(hr)) hr = piEncoder->CreateNewFrame(&piFrameEncode, NULL);

                // Initialize the encoder.
                if (SUCCEEDED(hr)) hr = piFrameEncode->Initialize(NULL);

                // Get and set the size.
                if (SUCCEEDED(hr)) hr = piFrameDecode->GetSize(&width, &height);
                if (SUCCEEDED(hr)) hr = piFrameEncode->SetSize(width, height);

                // Get and set the resolution.
                if (SUCCEEDED(hr)) piFrameDecode->GetResolution(&dpiX, &dpiY);
                if (SUCCEEDED(hr)) hr = piFrameEncode->SetResolution(dpiX, dpiY);

                // Set the pixel format.
                if (SUCCEEDED(hr)) piFrameDecode->GetPixelFormat(&pixelFormat);
                if (SUCCEEDED(hr)) hr = piFrameEncode->SetPixelFormat(&pixelFormat);

                // Check that the destination format and source formats are the same.
                bool formatsEqual = FALSE;
                if (SUCCEEDED(hr)) {
                    GUID srcFormat, destFormat;

                    hr = piDecoder->GetContainerFormat(&srcFormat);
                    if (SUCCEEDED(hr)) hr = piEncoder->GetContainerFormat(&destFormat);

                    if (SUCCEEDED(hr)) {
                        if (srcFormat == destFormat) formatsEqual = true;
                        else formatsEqual = false;
                    }
                }

                if (SUCCEEDED(hr) && formatsEqual) { // Copy metadata using metadata block reader/writer.
                    piFrameDecode->QueryInterface(IID_PPV_ARGS(&piBlockReader));
                    piFrameEncode->QueryInterface(IID_PPV_ARGS(&piBlockWriter));
                    piBlockWriter->InitializeFromBlockReader(piBlockReader);
                }

                if(SUCCEEDED(hr))   hr = piFrameEncode->GetMetadataQueryWriter(&piFrameQWriter);

                if (SUCCEEDED(hr)) { // Modify metadata.
                    PROPVARIANT value;
                    value.vt = VT_LPSTR;
                    value.pszVal=  const_cast<char *>(sTimestamp.c_str());
                    hr = piFrameQWriter->SetMetadataByName( L"/app1/ifd/exif/{ushort=36867}", &value);
                }

                if (SUCCEEDED(hr))
                    hr = piFrameEncode->WriteSource(
                        static_cast<IWICBitmapSource*> (piFrameDecode), NULL); // Using NULL enables JPEG loss-less encoding.

                // Commit the frame.
                if (SUCCEEDED(hr)) hr = piFrameEncode->Commit();

                if (piFrameDecode)  piFrameDecode->Release();
                if (piFrameEncode)  piFrameEncode->Release();
                if (piFrameQReader) piFrameQReader->Release();
                if (piFrameQWriter) piFrameQWriter->Release();
            }
        }

        if (SUCCEEDED(hr))  {
          piEncoder->Commit();
          piFileStream->Commit(STGC_DEFAULT);
        }

        if (piFileStream) piFileStream->Release();
        if (piEncoder) piEncoder->Release();
        if (piBlockWriter) piBlockWriter->Release();
        if (piBlockReader) piBlockReader->Release();

        piFactory->Release();
        piDecoder->Release();

        CoUninitialize( );

        std::cout << sFilename_new << "->" << sFilename << "\n";

        std::error_code ec;
        if (!SUCCEEDED(hr) || !fs::copy_file(sFilename_new, sFilename,  fs::copy_options::overwrite_existing, ec))
            std::cerr << "Error while copying file: "<< ec.message() << "\n";
        else
            fs::remove(sFilename_new);
    }
    else
      bStatus=false;
    
    return bStatus;
}


std::string spdFunc::stoyear(long long t) {
    std::stringstream ssS;
    
    constexpr long long M=60;
    constexpr long long H=60*M;
    constexpr long long D=24*H;
    constexpr long long Y=365*D;
    
    long long t0=-t;
    
    if (t<0)
        ssS << "-"
        << (  t0 / Y)               << "y "
        << (  t0 % Y) / D           << "d "
        << (( t0 % Y) % D) / H      << "h "
        << (((t0 % Y) % D) % H) / M << "m "
        << (((t0 % Y) % D) % H) % M << "s";
    else
        ssS << (  t / Y)           << "y "
        << (  t % Y) / D           << "d "
        << (( t % Y) % D) / H      << "h "
        << (((t % Y) % D) % H) / M << "m "
        << (((t % Y) % D) % H) % M << "s";
    
    return ssS.str();
}

bool spdFunc::test_ext(const std::string &sS) {
    bool res=false;
    for(const auto &ext: EXTENSION_LIST)
        res|=(sS.find(ext)!=std::string::npos);
    return res;
}

long long spdFunc::fileNb(const fs::path &path) {
    long long n=-1;
    try { n=std::distance(fs::recursive_directory_iterator{path}, fs::recursive_directory_iterator{}); }
    catch (fs::filesystem_error &err) {std::cerr << "- spdFunc::fileNb(): error: "  << ": " << err.what() << "\n";}
    return n;
}

std::string spdFunc::shiftTimestamp(const std::string &sTimestamp, long long t, bool bIsDST) {
    
    const std::string dateTimeFormat{ "%Y:%m:%d %H:%M:%S" };

    std::stringstream ssS(sTimestamp);

  if (sTimestamp.empty()) {
    ssS.clear();
    ssS << "1970:01:01 00:00:00";
  }

    std::tm dt={ };
    if (bIsDST) dt.tm_isdst=bIsDST;
    
    ssS >> std::get_time(&dt, dateTimeFormat.c_str());
    std::time_t iEpoch=std::mktime(&dt);
    
    iEpoch+=t;
    
    ssS.clear();
    ssS << std::put_time(std::localtime(&iEpoch), (dateTimeFormat).c_str());

    return ssS.str();
}
