XIncludeFile "open.pbi"

Procedure Open(file.s)
  Protected x.l, y.l, nb.l, nb2.l, len.l

  If ReadFile(0, file) = 0
    MessageRequester("ReadFile() error", "Can't load map.")
    ProcedureReturn
  EndIf

  map_\file = file
  isset = 2

  map_\title = ReadString(0)
  map_\w = ReadByte(0)
  map_\h = ReadByte(0)

  Global Dim map_data.s_map_tile(map_\w, map_\h)
  For y=0 To map_\h-1
    For x=0 To map_\w-1
      map_data(x, y)\id = ReadByte(0)
    Next x
  Next y

  Global Dim map_data_temp.s_map_tile(map_\w, map_\h)
  Global Dim map_data_temp2.s_map_tile(map_\w, map_\h)

  CloseFile(0)
EndProcedure
; IDE Options = PureBasic 5.30 (Windows - x86)
; CursorPosition = 20
; FirstLine = 1
; Folding = -
; EnableXP
; UseMainFile = main.pb
; CurrentDirectory = ..\
; EnablePurifier