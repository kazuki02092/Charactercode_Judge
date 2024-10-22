#include <stdio.h>
#include <stdlib.h>

int charactercode_judge(unsigned char *buf, int length){
    /* ISO-2022-JP(JIS)かどうかを判定 */
	for(int i=0;i<length;i++){
	    /* printf("%02x\n", buf[i]); */
	    if(buf[i]==0x1b){
	        return 1;
	    }
	}
    
	/* Shift_JIS、EUC-JPを判定 */
    int i=0;
    while(i<length){
        if(buf[i] >= 0x21 && buf[i] <= 0x7e){
			/* ASCIIなので判定不可 → 次の文字へ */
            i++;
        }else if(buf[i] >= 0x81 && buf[i] <= 0x9f){
			/* 第1バイトよりShift_JIS */
            return 2;
        }else if( (buf[i] >= 0xa1 && buf[i] <= 0xdf) || (buf[i] >= 0xf0 && buf[i] <= 0xfe) ){
			/* 第1バイトよりEUC-JP */
            return 3;
        }else if(buf[i+1] >= 0x40 && buf[i+1] <= 0xa0){
			/* 第2バイトよりShift_JIS */
            return 2;
        }else if(buf[i+1] >= 0xfd && buf[i+1] >= 0xfe){
			/* 第2バイトよりEUC-JP */
            return 3;
        }else{
			/* 第1,2バイトで判定不可 → 次の文字へ */
            i=i+2;
        }
    }

	return 0; //全文字で判定不可
}

int main(int argc, char *argv[]){
	int length, result;
	unsigned char *buf;
	FILE *fp;
	
	/* ファイルをバイナリ読み込みモードで開く */
	fp=fopen(argv[1],"rb"); 
	if(fp==NULL){
		printf("ファイルが見つかりません\n");
		exit(1);
	}
	
	/* ファイルサイズ(バイト数)を取得 */
	fseek(fp, 0, SEEK_END);  /* データ読み込み位置をファイル終端にセット */
	length=ftell(fp);        /* データ読み込み位置のファイル先頭からのバイト数を取得 */
	fseek(fp, 0, SEEK_SET);  /* データ読み込み位置をファイル先頭に戻す */
	
	/* 読み込むデータ(バイト列)を記録するための配列buf[0]～buf[length-1]を確保 */
	buf=malloc(length);
	
	/* ファイルからデータをバイト列として読み込みbuf[0]～buf[length-1]に格納 */
	fread(buf, 1, length, fp);
	fclose(fp);

    result = charactercode_judge(buf, length);
	switch(result){
		case 1:
			printf("ISO-2022-JPです\n");
			break;
		case 2:
			printf("Shift_JISです\n");
			break;
		case 3:
			printf("EUC-JPです\n");
			break;
		default:
			printf("文字コードの判別に失敗しました\n");
	}

    free(buf);
    return 0;
}