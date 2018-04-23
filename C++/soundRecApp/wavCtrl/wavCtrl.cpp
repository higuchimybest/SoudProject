/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   wavCtrl.cpp
 * Author: admin
 * 
 * Created on 2018/04/07, 21:23
 */

#include "wavCtrl.h"
#include <err.h>

wavCtrl::wavCtrl() {
}

wavCtrl::wavCtrl(const wavCtrl& orig) {
}

wavCtrl::~wavCtrl() {
}

/** 
 *  readWave
 *  概要  : wavの読み込み
 *  引数  : pcm       wav内容保存変数
 *  　　  : filepath  読み込みwavファイルパス
 *  戻り値: 実行結果
 */
bool wavCtrl::readWave(MONO_PCM *pcm, char *filepath) {
    int i;
    int offset;
 
    FILE *fp;
    if ( (fp=fopen(filepath,"rb")) == NULL ){
        printf("file open error.\n");
        return false;
    }
 
    int fmt_samples_per_sec;
    short fmt_blocksize;
    short fmt_bits_per_sample;
    int data_chunk_size;
 
    fseek(fp,24,SEEK_CUR);
    fread(&fmt_samples_per_sec,4,1,fp);
 
    fseek(fp,4,SEEK_CUR);
    fread(&fmt_blocksize,2,1,fp);
    fread(&fmt_bits_per_sample,2,1,fp);
 
    fseek(fp,4,SEEK_CUR);
    fread(&data_chunk_size,4,1,fp);

    pcm->fs = fmt_samples_per_sec;
    pcm->bits = fmt_bits_per_sample;
    pcm->length = data_chunk_size / fmt_blocksize;/* 総データ/1サンプル */
    
    pcm->s=(double*)calloc(pcm->length,sizeof(double));/*　メモリ確保　*/
    if( pcm->s == NULL ){
        printf("memory allocation fault.\n");
        return false;
    }
 
    if(fmt_bits_per_sample==16){
 
        /*offset=0;*/
        short data;
 
        for(i=0; i < pcm->length ; i++){
            fread(&data,2,1,fp);
            pcm->s[i]=data;
        }
 
    }else if(fmt_bits_per_sample==8){
         
        offset=128;
        unsigned char data;
 
        for(i=0; i < pcm->length ; i++){
            fread(&data,1,1,fp);
            pcm->s[i]=((short)data - offset);
        }
 
    }
 
    fclose(fp);
    
    return true;
}

/** 
 *  writeWave
 *  概要  : wavの読み込み
 *  引数  : pcm       wav内容保存変数
 *  　　  : filepath  書き込みwavファイルパス
 *  戻り値: 実行結果
 */
bool wavCtrl::writeWave(MONO_PCM *pcm, char *fileName)
{
    FILE *fp;
    int i;
    WAVE_FORMAT waveFormat;

    strcpy(waveFormat.riffChunk.chunkID, "RIFF");
    waveFormat.riffChunk.chunkSize = 36 + pcm->length * 2;
    strcpy(waveFormat.riffChunk.chunkFormType, "WAVE");

    strcpy(waveFormat.fmtChunk.chunkID, "fmt ");
    waveFormat.fmtChunk.chunkSize = 16;
    waveFormat.fmtChunk.waveFormatType = 1;
    // PCMの場合は1
    waveFormat.fmtChunk.formatChannel = 1;
    // モノラルの場合は1,ステレオの場合は2
    waveFormat.fmtChunk.samplesPerSec = pcm->fs;
    waveFormat.fmtChunk.bytesPerSec = (pcm->fs * pcm->bits) / 8;
    waveFormat.fmtChunk.blockSize = pcm->bits / 8;
    waveFormat.fmtChunk.bitsPerSample = pcm->bits;

    strcpy(waveFormat.dataChunk.chunkID, "data");
    waveFormat.dataChunk.chunkSize = pcm->length * 2;

    fp = fopen(fileName, "wb");

    if(!fp)
    {
        printf("file open error");
        return false;
    }

    fwrite(waveFormat.riffChunk.chunkID, 1, 4, fp);
    fwrite(&waveFormat.riffChunk.chunkSize, 4, 1, fp);
    fwrite(waveFormat.riffChunk.chunkFormType, 1, 4, fp);
    // RIFFチャンクの書き込み

    fwrite(waveFormat.fmtChunk.chunkID, 1, 4, fp);
    fwrite(&waveFormat.fmtChunk.chunkSize, 4, 1, fp);
    fwrite(&waveFormat.fmtChunk.waveFormatType, 2, 1, fp);
    fwrite(&waveFormat.fmtChunk.formatChannel, 2, 1, fp);
    fwrite(&waveFormat.fmtChunk.samplesPerSec, 4, 1, fp);
    fwrite(&waveFormat.fmtChunk.bytesPerSec, 4, 1, fp);
    fwrite(&waveFormat.fmtChunk.blockSize, 2, 1, fp);
    fwrite(&waveFormat.fmtChunk.bitsPerSample, 2, 1, fp);
    // fmtチャンクの書き込み

    fwrite(waveFormat.dataChunk.chunkID, 1, 4, fp);
    fwrite(&waveFormat.dataChunk.chunkSize, 4, 1, fp);

    short data;
    double s;
    for(i = 0; i < pcm->length; i++)
    {
        s = ((pcm->s[i] + 1.0) / 2) * (MAXPLUS + 1.0);
        if(s > MAXPLUS)
            s = MAXPLUS;
        else if(s < MAXMINUS)
            s = MAXMINUS;

        data = (short)(s + 0.5) - GROUND;
        fwrite(&data, 2, 1, fp);
    }

    fclose(fp);
    
    return true;
}

/** 
 *  csv_write
 *  概要  : CSVの出力
 *  引数  : pcm       wav内容保存変数
 *  　　  : filepath  書き込みcsvファイルパス
 *  戻り値: 実行結果
 */
bool wavCtrl::csv_write(MONO_PCM *pcm, char *file_name) { 
    int i;
    FILE *fp;
    
    if ( (fp=fopen(file_name,"w")) == NULL ){/*write only*/
        printf("file open error.\n");
        return false;
    }
 
    fprintf(fp, "%d,%d,%d\n", pcm->fs, pcm->bits, pcm->length);
 
    for(i=0; i < pcm->length ; i++){
        fprintf(fp,"%d,%f\n",i+1, pcm->s[i]);
    }
 
    fclose(fp);
    return true;
}

/** 
 *  runRec
 *  概要  : 録音起動
 *  引数  : pNowTime  日付文字列
 *  戻り値: 実行結果
 */
bool wavCtrl::runRec(const char *pNowTime) {
    int ret;
    char cFileName[100];
    char cCommand[100];

    cFileName[0] = 0x00;
    cCommand[0] = 0x00;

    strcpy(cFileName, pNowTime);
    strcat(cFileName, ".wav");
    cFileName[strlen(cFileName)] = 0x00;

    strcpy(cCommand, "arecord -D plughw:1,0 -d ");
    strcat(cCommand, DEFAULT_REC_TIME);
    strcat(cCommand, " -f cd ");
    strcat(cCommand, DEFAULT_DIR);
    strcat(cCommand, cFileName);
    cCommand[strlen(cCommand)] = 0x00;
    ret = system(cCommand);
    
    return WIFEXITED(ret);
}

//char *ExecCmd(const char* cmd) {
//    FILE *fp;
//    char buf[256];
//    
//    buf[0] = 0x00;
//    if ( (fp=popen(cmd, "r")) ==NULL) {
//        return false;
//    }
//    while(fgets(buf, sizeof(buf), fp) != NULL) {
//         (void) fputs(buf, stdout);
//    }
//    (void) pclose(fp);
//    
//    return buf;
//}

/** 
 *  runReadStats
 *  概要  : wavファイルから音声結果を取得
 *  引数  : pNowTime  日付文字列
 *  戻り値: 実行結果
 */
bool wavCtrl::runReadStats(const char *pNowTime) {
    int ret;
    char *pBuff = NULL;
    char cCommand[100];

    strcpy(cCommand, "python ");
    strcat(cCommand, DEFAULT_DIR_CORE);
    strcat(cCommand, "sound.py "); 
    strcat(cCommand, DEFAULT_DIR);
//    strcat(cCommand, pNowTime);
//    strcat(cCommand, ".wav"); 
    strcat(cCommand, " "); 
    strcat(cCommand, DEFAULT_DIR);
    //strcat(cCommand, " -n stats");
    cCommand[strlen(cCommand)] = 0x00;
    ret = system(cCommand);
    
    return WIFEXITED(ret);
}

/** 
 *  runOutCSV
 *  概要  : wavファイルからCSVを取得
 *  引数  : pNowTime  日付文字列
 *  戻り値: 実行結果
 */
bool wavCtrl::runOutCSV(const char *pNowTime) {
    int ret;
    MONO_PCM pcm;
    char cReadFileName[100];

    strcpy(cReadFileName, DEFAULT_DIR);
    strcat(cReadFileName, pNowTime);
    strcat(cReadFileName, ".wav");
    cReadFileName[strlen(cReadFileName)] = 0x00;
    if (readWave(&pcm, cReadFileName) != false) {

        cReadFileName[0] = 0x00;
        strcpy(cReadFileName, DEFAULT_DIR);
        strcat(cReadFileName, pNowTime);
        strcat(cReadFileName, ".csv");
        cReadFileName[strlen(cReadFileName)] = 0x00;
        csv_write(&pcm, cReadFileName);
    }
    
    return WIFEXITED(ret);
}