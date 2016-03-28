XIncludeFile "save.pbi"

Procedure SaveRequester(file.s)
  If file
    If FindString(file, ".map", 1) = 0
      file = file+".map"
    EndIf
    If FileSize(file) > 0
      If MessageRequester("Error", "File already exists. Replace it ?", #PB_MessageRequester_YesNo) = #PB_MessageRequester_Yes
        DeleteFile(file)
      Else
        ProcedureReturn
      EndIf
    EndIf
    Save(file)
  EndIf
EndProcedure

Procedure Save(file.s)
  Protected x.l, y.l
  
  If CreateFile(0, file) = 0
    MessageRequester("OpenFile() error", "Can't save map !")
    ProcedureReturn
  EndIf
  
  map_\file = file
  
  WriteStringN(0, map_\title)
  WriteByte(0, map_\w)
  WriteByte(0, map_\h)
  
  For y=0 To map_\h-1
    For x=0 To map_\w-1
      WriteByte(0, map_data(x, y)\id)
    Next x
  Next y
  
  CloseFile(0)
  isset = 2
EndProcedure
; IDE Options = PureBasic 5.30 (Windows - x86)
; CursorPosition = 26
; FirstLine = 9
; Folding = -
; EnableXP
; UseMainFile = main.pb
; CurrentDirectory = ..\
; EnablePurifier