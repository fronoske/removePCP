// removePCP.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <sys/stat.h>
#include <windows.h>

#define HSIZE 14

int _tmain(int argc, _TCHAR* argv[])
{
  FILE *fpr = (FILE*)NULL;
  FILE *fpw = (FILE*)NULL;
  TCHAR fname_r[MAX_PATH];
  TCHAR fname_w[MAX_PATH];
  BYTE hbuf[HSIZE];
  BYTE *cbuf = (BYTE*)NULL;
  DWORD size=0UL;
  DWORD res = 0UL;
  errno_t error;
  
  struct _stat64 fstat;
  __int64 pos, total_size;

  if (argc != 3){
	  _tprintf(_T("USAGE: %s in.ts out.ts\n"), argv[0]);
	  return 1;
  }else{
	  _tcsncpy(fname_r, argv[1], MAX_PATH);
	  _tcsncpy(fname_w, argv[2], MAX_PATH);
  }
  error = _tfopen_s( &fpr, fname_r, _T("rb") );
  if( fpr == NULL ){
    _tprintf( _T("ERROR: Could not open input file %s.\n"), fname_r );
    _tprintf( _T("error number: %d\n"), error );
    return -1;
  }
  error = _tfopen_s(&fpw, fname_w, _T("wb") );
  if( fpw == NULL ){
    _tprintf( _T("ERROR: Could not open output file %s.\n"), fname_w );
    _tprintf( _T("error number: %d\n"), error );
	if(fpr) fclose(fpr);
	if(fpw) fclose(fpw);
    return -1;
  }

  res = _tstat64(fname_r, &fstat);
  total_size = fstat.st_size;
  _tprintf(_T("Total size = %13I64d Bytes\n"), total_size);

  while(HSIZE == (res = fread( hbuf, sizeof(BYTE), HSIZE, fpr ))){
	_tprintf(_T("%02x %02x %02x %02x: "), hbuf[10], hbuf[11], hbuf[12], hbuf[13]);
    size =(hbuf[10] << 24) + (hbuf[11] << 16) + (hbuf[12] << 8) + (hbuf[13]);
//	_tprintf(_T("will read %ld bytes.\n"), size);
	if (cbuf != (BYTE*)NULL) free(cbuf);
	cbuf = (BYTE *)malloc(size);
	if(cbuf == (BYTE*)NULL){
		_tprintf(_T("Memory allocation error! size=%ld"), size);
		break;
	}
	res = fread(cbuf, sizeof(BYTE), size, fpr);
        if (res == size){
          fwrite( cbuf, sizeof(BYTE), size, fpw );
	  pos = _ftelli64(fpr);
	  _tprintf(_T("%13I64d bytes (%3.0f%%) read.\r"), pos, ((long double)pos/total_size)*100);
	}else{
	  _tprintf(_T("\nERROR!! PCP header tells to read %ld bytes but actually could read %ld bytes.\n"), size, res);
	  break;
	}
  }
  if (cbuf != (BYTE*)NULL) free(cbuf);

  fclose( fpr );
  fclose( fpw );

  if (pos == total_size){
    _tprintf( _T("\nSuccessfully finished.\n"));
  }else{
    _tprintf( _T("\nAbnormally finished.\n"));    
  }
  return 0;
}

