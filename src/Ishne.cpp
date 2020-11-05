#include "Ishne.h"
#include <stdio.h>
#include <vector>

Ishne::Ishne() : mData(nullptr), mFileData(nullptr), mFileSize(0), mSamples(nullptr), mVarblock(nullptr) {
    
}

Ishne::~Ishne() {
    unload();
    unInit();
}

bool Ishne::init(const std::string &fileName) {
    unInit();
    
    mData = new ISNEHeader();
    
    unsigned char *data = (unsigned char *)load(fileName);
    
    if(nullptr!=data) {
        unsigned char *current_ptr = data;
        
        unsigned char magicnumber[8]={0};
        unsigned short checksum(0);
        long currentByteCount(0);
        
        memcpy(&magicnumber, current_ptr, sizeof(char)*8);
        current_ptr+=8;
        currentByteCount+=8;
        
        memcpy(&checksum, current_ptr, sizeof(char)*2);
        current_ptr+=2;
        currentByteCount+=2;
        
        memcpy(&mData->Var_length_block_size, current_ptr, sizeof(char)*4);
        current_ptr+=4;
        currentByteCount+=4;
        
        memcpy(&mData->Sample_Size_ECG, current_ptr, sizeof(char)*4);
        current_ptr+=4;
        currentByteCount+=4;
        
        memcpy(&mData->Offset_var_lenght_block, current_ptr, sizeof(char)*4);
        current_ptr+=4;
        currentByteCount+=4;
        
        memcpy(&mData->Offset_ECG_block, current_ptr, sizeof(char)*4);
        current_ptr+=4;
        currentByteCount+=4;
        
        memcpy(&mData->File_Version, current_ptr, sizeof(char)*2);//28 bytes
        current_ptr+=2;
        currentByteCount+=2;
        
        memcpy(&mData->First_Name, current_ptr, sizeof(char)*40); // 68 bytes
        current_ptr+=40;
        currentByteCount+=40;
        
        memcpy(&mData->Last_Name, current_ptr, sizeof(char)*40); // 108 bytes
        current_ptr+=40;
        currentByteCount+=40;
        
        memcpy(&mData->ID, current_ptr, sizeof(char)*20); // 128 bytes
        current_ptr+=20;
        currentByteCount+=20;
        
        memcpy(&mData->Sex, current_ptr, sizeof(char)*2);
        current_ptr+=2;
        currentByteCount+=2;
        
        memcpy(&mData->Race, current_ptr, sizeof(char)*2);
        current_ptr+=2;
        currentByteCount+=2;
        
        memcpy(&mData->Birth_Date, current_ptr, sizeof(char)*6); // 138 bytes
        current_ptr+=6;
        currentByteCount+=6;
        
        memcpy(&mData->Record_Date, current_ptr, sizeof(char)*6); // 144 bytes
        current_ptr+=6;
        currentByteCount+=6;
        
        memcpy(&mData->File_Date, current_ptr, sizeof(char)*6); // 150 bytes
        current_ptr+=6;
        currentByteCount+=6;
        
        memcpy(&mData->Start_Time, current_ptr, sizeof(char)*6); // 156 bytes
        current_ptr+=6;
        currentByteCount+=6;
        
        memcpy(&mData->nLeads, current_ptr, sizeof(char)*2); // 158 bytes
        current_ptr+=2;
        currentByteCount+=2;
        
        memcpy(&mData->Lead_Spec, current_ptr, sizeof(char)*24); // 182 bytes
        current_ptr+=24;
        currentByteCount+=24;
        
        memcpy(&mData->Lead_Qual, current_ptr, sizeof(char)*24); // 206 bytes
        current_ptr+=24;
        currentByteCount+=24;
        
        memcpy(&mData->Resolution, current_ptr, sizeof(char)*24); // 230 bytes
        current_ptr+=24;
        currentByteCount+=24;
        
        memcpy(&mData->Pacemaker, current_ptr, sizeof(char)*2); // 232 bytes
        current_ptr+=2;
        currentByteCount+=2;
        
        memcpy(&mData->Recorder, current_ptr, sizeof(char)*40); // 272 bytes
        current_ptr+=40;
        currentByteCount+=40;
        
        memcpy(&mData->Sampling_Rate, current_ptr, sizeof(char)*2); // 274 bytes
        current_ptr+=2;
        currentByteCount+=2;
        
        memcpy(&mData->Proprietary, current_ptr, sizeof(char)*80); // 354 bytes
        current_ptr+=80;
        currentByteCount+=80;
        
        memcpy(&mData->Copyright, current_ptr, sizeof(char)*80); // 434 bytes
        current_ptr+=80;
        currentByteCount+=80;
        
        memcpy(&mData->Reserved, current_ptr, sizeof(char)*88); // 522 bytes
        current_ptr+=88;
        currentByteCount+=88;
        
        mVarblock=nullptr;
        if(mData->Var_length_block_size > 0) {
            char *varblock = new char[mData->Var_length_block_size];
            memcpy(varblock, current_ptr, sizeof(char)*mData->Var_length_block_size); // 522 bytes
            current_ptr+=mData->Var_length_block_size;
            currentByteCount+=mData->Var_length_block_size;
        }
        
        mSamples = new SampleVector;
        
        for(int i = 0; i < mData->Sample_Size_ECG; i++) {
            LeadVector leadVector;
            
            for(int j = 0; j < mData->nLeads && (currentByteCount < mFileSize); j++) {
                short data;
                memcpy(&data, current_ptr, sizeof(char)*2); // 522 bytes
                current_ptr+=2;
                currentByteCount+=2;
                
                leadVector.push_back(data);
            }
            mSamples->push_back(leadVector);
            
            if(currentByteCount >= mFileSize) {
                
                long specification_file_size = 8 + 2 + 512 + mData->Var_length_block_size + (mData->Sample_Size_ECG * mData->nLeads);
                
                std::string err_str = "The file `";
                err_str += fileName;
                err_str += "` was corrupt. According to the file specifications the file should be ";
                err_str += std::to_string(specification_file_size);
                err_str += " bytes, but the file is ";
                err_str += std::to_string(mFileSize);
                err_str += " bytes. Which is ";
                
                float diff(specification_file_size - mFileSize);
                std::string units(" ");
                if(diff > 1000000000) {
                    diff /= 1000000000.;
                    units = " Giga";
                }else if(diff > 1000000) {
                    diff /= 1000000.;
                    units = " Mega";
                }else if(diff > 1000) {
                    diff /= 1000.;
                    units = " Kilo";
                }
                err_str += std::to_string(diff);
                err_str += units;
                err_str += "bytes less then what is expected, according to what is in the specifications.";
                
                unload();
                
                throw std::runtime_error(err_str);
            }
        }
        
        unload();
        
        return true;
    }
    
    return false;
}

void Ishne::unInit() {
    if(nullptr!=mData)delete mData;
    mData = nullptr;
    if(nullptr!=mSamples)delete mSamples;
    mSamples = nullptr;
    if(nullptr!=mVarblock)delete mVarblock;
    mVarblock = nullptr;
}

long Ishne::numberOfSamples()const {
    if(mSamples)return mSamples->size();
    return 0;
}

short Ishne::numberOfLeads()const {
    if(mData)return mData->nLeads;
    return 0;
}

short Ishne::samplingRate()const {
    if(mData)return mData->Sampling_Rate;
    return 0;
}

short Ishne::getSample(const size_t sample_index, const size_t lead_index)const {
    if(mData) {
        if(sample_index < mSamples->size()) {
            if(lead_index < mSamples->at(sample_index).size()) {
                return mSamples->at(sample_index).at(lead_index);
            }
        }
    }
    return 0;
}

void *Ishne::load(const std::string &fileName) {
    unload();
    FILE *file = fopen(fileName.c_str(), "rb");
    if(file) {
        fseek(file, 0, SEEK_END);
        mFileSize = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        mFileData = malloc(mFileSize+1);
        fread(mFileData, 1, mFileSize, file);
        fclose(file);
        
        return mFileData;
    }
    return nullptr;
}

void Ishne::unload() {
    if(nullptr!=mFileData)free(mFileData);
    mFileData = nullptr;
    mFileSize = 0;
}
