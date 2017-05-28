.386
.model flat, stdcall  ;32 bit memory model
option casemap :none  ;case sensitive

include FirstDlg.inc

.code

start:
	invoke GetModuleHandle,NULL
	mov		hInstance,eax

    invoke InitCommonControls
	invoke DialogBoxParam,hInstance,IDD_INJECT,NULL,addr DlgProc,NULL
	invoke ExitProcess,0

;########################################################################

DlgProc proc hWin:HWND,uMsg:UINT,wParam:WPARAM,lParam:LPARAM

	mov		eax,uMsg
	.if eax==WM_INITDIALOG

	.elseif eax==WM_COMMAND
		mov eax, wParam
		.if eax == BTN_INJECT
			invoke InjectCode
		.elseif eax == IDC_BTN1
		        invoke MyFreeMem
		.endif
	.elseif eax==WM_CLOSE
		invoke EndDialog,hWin,0
	.else
		mov		eax,FALSE
		ret
	.endif
	mov		eax,TRUE
	ret

DlgProc endp

begin_label: 
	push 12345678h
        pushfd
        pushad
	call $+5  ;重定位
FIXADDR:
	pop ebp
	sub ebp, FIXADDR
	push MB_OK
	
	lea eax, [ebp + offset g_szTitle]
	push eax
	
	lea eax, [ebp + offset g_szMsg]
	push eax
	
	push NULL
	mov eax, [ebp + offset g_pfnMessageBox]
	call eax
	popad
	popfd
	ret
	g_szTitle db 'Title', 0
	g_szMsg db 'Hello World', 0
	g_pfnMessageBox DWORD 0
	
end_label:


InjectCode proc 
	LOCAL @hCalc:HWND
	LOCAL @dwPid :dword
	LOCAL @dwTid:dword
	LOCAL @hProcess :HANDLE
	LOCAL @lpBuff :PVOID
	LOCAL @hUser:HANDLE
	LOCAL @oldProtect:DWORD
	LOCAL @lpMsgBox:PVOID
	LOCAL @dwProcessID:dword
	LOCAL @dwThreadID:dword
	LOCAL @te32:THREADENTRY32 
	LOCAL @hThreadSnap:HANDLE
	LOCAL @hThread:HANDLE
	LOCAL @Context:CONTEXT 
	
	
	invoke LoadLibrary, addr g_szUser32
	mov @hUser, eax
	;check
	.if eax == NULL
	    invoke MessageBox,NULL,offset g_szUser32,offset g_err,MB_OK
	    ret
	.endif
	
	
	invoke GetProcAddress, @hUser, addr g_szMsgBox
	mov @lpMsgBox, eax
	
	;check
	.if eax == NULL
	    invoke MessageBox,NULL,offset g_szGetProcAddress,offset g_err,MB_OK
	    ret
	.endif
	
	
	;修改内存保护属性
	invoke VirtualProtect, addr begin_label,  end_label -  begin_label, \
			PAGE_EXECUTE_READWRITE, addr @oldProtect
	;check
	.if eax == NULL
	    invoke MessageBox,NULL,offset g_szVirtualProtect,offset g_err,MB_OK
	    ret
	.endif
	
	mov eax, @lpMsgBox		
	mov g_pfnMessageBox, eax
	
	
		
	invoke FreeLibrary,@hUser
	;check
	.if eax == NULL
	    invoke MessageBox,NULL,offset g_szFreeLibrary,offset g_err,MB_OK
	    ret
	.endif
	
	invoke FindWindow,NULL, addr g_szCalc
	mov @hCalc, eax
	;check
	.if eax == NULL
	    invoke MessageBox,NULL,offset g_szFindWindow,offset g_err,MB_OK
	    ret
	.endif
	
	invoke GetWindowThreadProcessId,@hCalc, addr @dwPid
	;check
	mov ebx,@dwPid
	mov @dwTid,eax
	.if ebx == NULL
	    invoke MessageBox,NULL,offset g_szGetWindowThreadProcessId,offset g_err,MB_OK
	    ret
	.endif
	
	

	invoke OpenProcess, PROCESS_ALL_ACCESS, FALSE, @dwPid
	mov @hProcess, eax
	mov g_hProcess,eax
	;check
	.if eax == NULL
	    invoke MessageBox,NULL,offset g_szOpenProcess,offset g_err,MB_OK
	    ret
	.endif
	
	
	;申请内存
	invoke VirtualAllocEx, @hProcess, NULL, 1000h, MEM_COMMIT, PAGE_EXECUTE_READWRITE
	mov @lpBuff, eax
	mov g_lpBuff,eax
	mov g_szAddr,eax
	;check
	.if eax == NULL
	    invoke MessageBox,NULL,offset g_szVirtualAllocEx,offset g_err,MB_OK
	    ret
	.endif
	
	
	;写入内存
	invoke WriteProcessMemory,@hProcess, @lpBuff, \
		addr begin_label, end_label - begin_label, NULL
	;check
	.if eax == 0
	    invoke MessageBox,NULL,offset g_szWriteProcessMemory,offset g_err,MB_OK
	    ret
	.endif
	
		invoke CreateToolhelp32Snapshot,TH32CS_SNAPTHREAD,0
	mov @hThreadSnap,eax
	.IF eax == INVALID_HANDLE_VALUE 
	    invoke MessageBox,NULL,offset g_szCreateToolhelp32Snapshot,offset g_err,MB_OK
	    ret
	.ENDIF
	
    
	mov eax,sizeof @te32
	mov @te32.dwSize,eax
	invoke Thread32First,@hThreadSnap,addr @te32
	.IF eax == TRUE
	.WHILE eax == TRUE
	    mov eax,@te32.th32OwnerProcessID
	    .IF @dwPid == eax
	        mov ebx,@te32.th32ThreadID
	        mov @dwThreadID,ebx
	        .BREAK
	    .ENDIF
	    invoke Thread32Next,@hThreadSnap,addr @te32
	.ENDW
	
	.ENDIF
    
    
	invoke OpenThread,THREAD_ALL_ACCESS,FALSE,@dwThreadID
	mov @hThread,eax
	
	
	invoke SuspendThread,@hThread
	.if eax == -1
	    invoke MessageBox,NULL,offset g_szSuspendThread,offset g_err,MB_OK
	    ret
	.endif
	
	mov @Context.ContextFlags,CONTEXT_FULL
	invoke GetThreadContext,@hThread,addr @Context
	.if eax == 0
	    invoke MessageBox,NULL,offset g_szGetThreadContext,offset g_err,MB_OK
	    ret
	.endif
	
	push ecx
	mov ecx,@Context.regEip
	mov g_pOldEip,ecx
	pop ecx
	
	mov eax,@lpBuff
	mov @Context.regEip,eax

	
	
	
	mov eax,@lpBuff
	add eax,1
	
	;写入内存
	invoke WriteProcessMemory,@hProcess, eax, \
		offset g_pOldEip,4, NULL
	;check
	.if eax == 0
	    invoke MessageBox,NULL,offset g_szWriteProcessMemory,offset g_err,MB_OK
	    ret
	.endif
	
	
	
	invoke SetThreadContext,@hThread,addr @Context
	
	.if eax == 0
	    invoke MessageBox,NULL,offset g_szSetThreadContext,offset g_err,MB_OK
	    ret
	.endif
	
	invoke ResumeThread,@hThread
	.if eax == -1
	    invoke MessageBox,NULL,offset g_szResumeThread,offset g_err,MB_OK
	    ret
	.endif
	;创建远程线程
	;invoke CreateRemoteThread,@hProcess, NULL, 0, @lpBuff, NULL, 0, NULL
	;check
	;.if eax == NULL
	;   invoke MessageBox,NULL,offset g_szCreateRemoteThread,offset g_err,MB_OK
	;    ret
	;.endif
	
	
	
	;修改完一次过后需要再次修改最开始的状态
	invoke VirtualProtect, addr begin_label,  end_label -  begin_label, \
			@oldProtect, addr @oldProtect
	;check
	.if eax == NULL
	    invoke MessageBox,NULL,offset g_szVirtualProtect,offset g_err,MB_OK
	    ret
	.endif
	
	
	
	ret
    
InjectCode endp

MyFreeMem proc
	;释放内存
	invoke VirtualFreeEx,g_hProcess, g_lpBuff, 0,MEM_RELEASE
	;check
	.if eax == 0
	    invoke MessageBox,NULL,offset g_szVirtualFreeEx,offset g_err,MB_OK
	    ret
	.endif
	
	ret

MyFreeMem endp
end start
