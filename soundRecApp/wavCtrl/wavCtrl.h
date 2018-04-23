/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   wavCtrl.h
 * Author: admin
 *
 * Created on 2018/04/07, 21:22
 */

#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../defaultConf.h"

#define GROUND 32768.0
#define MAXPLUS 65536.0
#define MAXMINUS 0.0

#ifndef WAVECTRL_H
#define WAVECTRL_H

typedef struct
{
    int fs;
    // 標本化周波数
    int bits;
    // 量子化制度
    int length;
    // データ長
    double *s;
    // 音データ
} MONO_PCM;
// モノラルの音声データの定義

typedef struct
{
    char chunkID[4];
    long chunkSize;
    char chunkFormType[4];
} RIFF_CHUNK;
// RIFFチャンクの定義

typedef struct
{
    char chunkID[4];
    long chunkSize;
    short waveFormatType;
    short formatChannel;
    long samplesPerSec;
    long bytesPerSec;
    short blockSize;
    short bitsPerSample;
} FMT_CHUNK;
// fmtチャンクの定義

typedef struct
{
    char chunkID[4];
    long chunkSize;
    short data;
} DATA_CHUNK;
// dataチャンクの定義

typedef struct
{
    RIFF_CHUNK riffChunk;
    FMT_CHUNK fmtChunk;
    DATA_CHUNK dataChunk;
} WAVE_FORMAT;
// WAVEフォーマット

class wavCtrl {
public:
    wavCtrl();
    wavCtrl(const wavCtrl& orig);
    virtual ~wavCtrl();
    
    bool runRec(const char *);
    bool runReadStats(const char *);
    bool runOutCSV(const char *);
private:

    bool readWave(MONO_PCM*, char*);
    bool writeWave(MONO_PCM*, char*);
    bool csv_write(MONO_PCM *, char *);
};

#endif /* WAVCTRL_H */

