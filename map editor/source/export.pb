XIncludeFile "export.pbi"

Procedure ExportRequester(file.s)
  If file
    If FindString(file, ".DAT", 1) = 0
      file = file + ".DAT"
    EndIf
    If FileSize(file) > 0
      If MessageRequester("Error", "File already exists. Replace it ?", #PB_MessageRequester_YesNo) = #PB_MessageRequester_Yes
        DeleteFile(file)
      Else
        ProcedureReturn
      EndIf
    EndIf
    ExportMap(file)
  EndIf
EndProcedure

Procedure ExportMap(file.s)
;   Protected x.l, y.l
;   
;   If CreateFile(0, file) = 0
;     MessageRequester("OpenFile() error", "Can't export map.")
;     ProcedureReturn
;   EndIf
;   
;   WriteStringN(0, "map_"+ReplaceString(LCase(map_\title), " ", "_", #PB_String_InPlace)+":")
;   WriteStringN(0, Chr(9)+".db "+"$"+RSet(Hex(map_\w), 2, "0")+",$"+RSet(Hex(map_\h), 2, "0"))
;   For y=0 To map_\h-1
;     WriteString(0, Chr(9)+".db ")
;     For x=0 To map_\w-1
;       WriteString(0, "$"+RSet(Hex(map_data(x, y)\id), 2, "0"))
;       If x<>map_\w-1
;         WriteString(0, ",")
;       EndIf
;     Next x
;     WriteStringN(0, "")
;   Next y
;   CloseFile(0)

  Protected x.l, y.l

  If CreateFile(0, file) = 0
    MessageRequester("OpenFile() error", "Can't export map !")
    ProcedureReturn
  EndIf

  map_\file = file

  WriteByte(0, map_\w)
  WriteByte(0, map_\h)

  For y=0 To map_\h-1
    For x=0 To map_\w-1
      WriteByte(0, map_data(x, y)\id)
    Next x
  Next y

  CloseFile(0)
EndProcedure
; IDE Options = PureBasic 5.30 (Windows - x86)
; CursorPosition = 43
; FirstLine = 32
; Folding = -
; EnableXP
; UseMainFile = main.pb
; CurrentDirectory = ..\
; EnablePurifier