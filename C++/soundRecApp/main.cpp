/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: admin
 *
 * Created on 2018/04/07, 2:09
 */

#include <cstdlib>
#include "main.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>
#include <unistd.h>
#include <signal.h>
#include "defaultConf.h"
#include "./wavCtrl/wavCtrl.h"
#include "wavCtrl/wavCtrl.h"

/* The following codes are only sample */
static volatile int sigterm = 0;

using namespace std;


static void handle_sigterm(int sig) { sigterm = 1; }

/** 
 *  getTimeBuff
 *  概要  : 本日日付の文字列を取得する
 *  引数  : なし
 *  戻り値: 本日日付の文字列
 */
char *getTimeBuff() {
    time_t timer;
    struct tm *tm;
    char cNowTime[100];
    char *pResult = NULL;
    
    // 現在時刻を取得
    timer = time(NULL);
    // 地方時に変換
    tm = localtime(&timer);
    // 時間の文字列に変換
    sprintf(cNowTime, "%04d%02d%02d_%02d%02d%02d",tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,tm->tm_hour, tm->tm_min, tm->tm_sec);
    cNowTime[strlen(cNowTime)] = 0x00;
    
    if ((pResult = (char*)malloc(strlen(cNowTime) + 1)) != NULL) {
        strcpy(pResult, cNowTime);
    }
    
    return pResult;
}

/** 
 *  mydaemon
 *  概要  : デーモンの起動
 *  引数  : なし
 *  戻り値: 本日日付の文字列
 */
static int mydaemon( void )
{
    syslog(LOG_INFO, "mydaemon started.\n");
    signal(SIGTERM, handle_sigterm);
    while(!sigterm) {

        char *pNowTime = getTimeBuff();
        
        if (pNowTime != NULL) {
            wavCtrl *pWavCtrlClass = new wavCtrl;

            if (pWavCtrlClass) {

                // 録画の実行
                if(pWavCtrlClass->runRec(pNowTime)){

                    // 音声解析の実行
                    if(pWavCtrlClass->runReadStats(pNowTime)){

                    }   
                }
                delete pWavCtrlClass;
            }

            free(pNowTime);
        }

        usleep(1000000);
    }
    syslog(LOG_INFO, "mydaemon stopped.\n");
    return 0;
}

/** 
 *  createSoundDir
 *  概要  : フォルダーを作成する
 *  引数  : なし
 *  戻り値: なし
 */
void createSoundDir() {
    struct stat st;
    char cNewDir[100];
    
    strcpy(cNewDir , DEFAULT_DIR);
    if(stat(cNewDir, &st) != 0){
         mkdir(cNewDir, 777);
    }
}

/** 
 *  main
 *  概要  : メイン関数
 *  引数  : argc  引数パラメータ
 *  　　    argv  引数パラメータ
 *  戻り値: 0
 */
int main(int argc, char** argv) {

    // Soundフォルダーがない場合に作成
    createSoundDir();
    
    //while (true) {
        char *pNowTime = getTimeBuff();

        if (pNowTime != NULL) {
            wavCtrl *pWavCtrlClass = new wavCtrl;

            if (pWavCtrlClass) {

                // 録画の実行
                if(pWavCtrlClass->runRec(pNowTime)){

                    // 音声解析の実行
                    if(pWavCtrlClass->runReadStats(pNowTime)){

                    }   
                }
                delete pWavCtrlClass;
            }

            free(pNowTime);
        }
    //}
    
    return 0;
}

