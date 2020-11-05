#include <string>

class Ishne {
    struct ISNEHeader{
        long Var_length_block_size;
        long Sample_Size_ECG;
        long Offset_var_lenght_block;
        long Offset_ECG_block;
        short int File_Version;
        unsigned char First_Name[40];
        unsigned char Last_Name[40];
        unsigned char ID[20];
        short int Sex;
        short int Race;
        short int Birth_Date[3];
        short int Record_Date[3];
        short int File_Date[3];
        short int Start_Time[3];
        short int nLeads;
        short int Lead_Spec[12];
        short int Lead_Qual[12];
        short int Resolution[12];
        short int Pacemaker;
        unsigned char Recorder[40];
        short int Sampling_Rate;
        unsigned char Proprietary[80];
        unsigned char Copyright[80];
        unsigned char Reserved[88];
    };
    ISNEHeader *mData;
    void *mFileData;
    long mFileSize;
    
    using LeadVector = std::vector<short>;
    using SampleVector = std::vector<LeadVector>;
    
    SampleVector *mSamples;
    char *mVarblock;
public:
    Ishne();
    ~Ishne();
    
    bool init(const std::string &fileName);
    void unInit();
    
    Ishne(const Ishne &rhs) = delete;
    Ishne(const Ishne &&rhs) = delete;
    const Ishne& operator=(const Ishne &rhs) = delete;
    const Ishne&& operator=(const Ishne &&rhs) = delete;
    
    long numberOfSamples()const;
    short numberOfLeads()const;
    short samplingRate()const;
    short getSample(const size_t sample_index, const size_t lead_index)const;
protected:
    void *load(const std::string &fileName);
    void unload();
};
