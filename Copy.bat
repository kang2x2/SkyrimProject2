// 명령어		옵션		원본파일 위치					사본파일 저장 위치
	
xcopy		/y/s	.\Engine\Public\*.h						.\Reference\Header\		
xcopy		/y		.\Engine\Bin\*.lib						.\Reference\Library\		
xcopy		/y		.\Engine\ThirdPartyLib\*.lib			.\Reference\Library\
xcopy		/y		.\Engine\Bin\*.dll						.\Client\Bin\
