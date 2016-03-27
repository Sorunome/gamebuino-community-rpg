EnableExplicit

If OpenConsole() = 0
  MessageRequester("OpenConsole() error","Unable to open the console.",0)
  End
EndIf

Define i.l
Global str1$
For i = 1 To 15
  str1$ = str1$ + Chr(205)
Next
ConsoleTitle ("img2c")
EnableGraphicalConsole(1)
PrintN(Chr(201)+str1$+Chr(187))
PrintN(Chr(186)+" img2c - deeph "+Chr(186))
PrintN(Chr(200)+str1$+Chr(188))
PrintN("")

i = CountProgramParameters()
If i > 1
  Define file_name.s = ProgramParameter()
  i-1
  If CreateFile(0, file_name)
    Define output_type.s = ProgramParameter()
    i-1
    If output_type = "-h" Or output_type = "-d" Or output_type = "-b" Or output_type = "-hr" Or output_type = "-dr" Or output_type = "-br" Or output_type = "-tr"
      While i > 0
        Define file.s = ProgramParameter()
        If GetExtensionPart(file) = "jpg" Or GetExtensionPart(file) = "jpeg"
          UseJPEG2000ImageDecoder()
        ElseIf GetExtensionPart(file) = "png"
          UsePNGImageDecoder()
        ElseIf GetExtensionPart(file) = "tif"
          UseTIFFImageDecoder()
        ElseIf GetExtensionPart(file) = "tga"
          UseTGAImageDecoder()
        ElseIf GetExtensionPart(file) <> "bmp"
          PrintN("Error : image format unsupported.")
          Input()
          End
        EndIf
        If LoadImage(0, file)
          If Mod(ImageWidth(0), 8) = 0
            Define y.l, x.l, y2.l, x2.l, bin.s
            WriteStringN(0, "const byte "+ReplaceString(GetFilePart(file), "."+GetExtensionPart(file), "")+"[]={")
            If StartDrawing(ImageOutput(0))
              If output_type = "-h" Or output_type = "-d" Or output_type = "-b"
                WriteString(0, "  "+ImageWidth(0)+","+ImageHeight(0)+",")
                For y = 0 To ImageHeight(0)-1
                  For x = 0 To ImageWidth(0)-1 Step 8
                    bin = ""
                    For x2 = 0 To 7
                      If Point(x+x2, y)
                        bin+"0"
                      Else
                        bin+"1"
                      EndIf
                    Next x2
                    Select output_type
                      Case "-h"
                        WriteString(0, "0x"+LCase(RSet(Hex(Val("%"+bin)), 2, "0")))
                      Case "-d"
                        WriteString(0, Str(Val("%"+bin)))
                      Case "-b"
                        WriteString(0, "B"+bin)
                    EndSelect
                    If x+x2 <> ImageWidth(0)
                      WriteString(0, ",")
                    EndIf
                  Next x
                  If y <> ImageHeight(0)-1
                    WriteString(0, ",")
                  EndIf
                Next y
              ElseIf output_type = "-hr" Or output_type = "-dr" Or output_type = "-br"
                WriteString(0, Chr(9))
                For x = 0 To 7
                  bin = ""
                  For y = 7 To 0 Step -1
                    If Point(x, y)
                      bin+"0"
                    Else
                      bin+"1"
                    EndIf
                  Next y
                  Select output_type
                    Case "-hr"
                      WriteString(0, "0x"+LCase(RSet(Hex(Val("%"+bin)), 2, "0")))
                    Case "-dr"
                      WriteString(0, Str(Val("%"+bin)))
                    Case "-br"
                      WriteString(0, "B"+bin)
                  EndSelect
                  If x <> 7
                    WriteString(0, ",")
                  Else
                    WriteString(0, "")
                  EndIf
                Next x
              Else
                For y = 0 To ImageHeight(0)-1 Step 8
                  WriteString(0, Chr(9))
                  For x = 0 To 7
                    bin = ""
                    For y2 = 7 To 0 Step -1
                      If Point(x, y+y2)
                        bin+"0"
                      Else
                        bin+"1"
                      EndIf
                    Next y2
                    WriteString(0, "0x"+LCase(RSet(Hex(Val("%"+bin)), 2, "0")))
                    If x <> 7
                      WriteString(0, ",")
                    Else
                      WriteString(0, "")
                    EndIf
                  Next x
                  If y <> ImageHeight(0)-1
                    WriteStringN(0, ",")
                  EndIf
                Next y
              EndIf
              WriteStringN(0, " };")
              StopDrawing()
            EndIf
          Else
            PrintN("Error : the image width is not aligned (8-multiple)")
            Input()
          EndIf
        Else
          PrintN("Error : unable to open the image.")
          Input()
        EndIf
        i-1
      Wend
      PrintN(file_name+" correctly generated.")
      CloseFile(0)
    Else
      PrintN("Error : parameters are : <file name> <-h|-d|-b|-hr|-dr|-br|-tr> <sprite images>")
      PrintN("")
      PrintN("file name : output file name")
      PrintN("-h : hexadecimal output format")
      PrintN("-d : decimal output format")
      PrintN("-b : binary output format")
      PrintN("-hr : hexadecimal output format with 90° clockwise rotation (useful for direct draw routines, 8*8 sprites only)")
      PrintN("-dr : decimal output format with 90° clockwise rotation (useful for direct draw routines, 8*8 sprites only)")
      PrintN("-br : binary output format with 90° clockwise rotation (useful for direct draw routines, 8*8 sprites only)")
      PrintN("-tr : tilesets with 90° clockwise rotation (useful for direct draw routines)")
      PrintN("")
      PrintN("sprite images : input sprite image (*.jpg|*.jpeg|*.png|*.tif|*.tga|*.bmp)")
      Input()
    EndIf
  Else
    PrintN("Error : unable to create the output file.")
    Input()
  EndIf
Else
  PrintN("Error : parameters are : <file name> <-h|-d|-b|-hr|-dr|-br|-tr> <sprite images>")
  PrintN("")
  PrintN("file name : output file name")
  PrintN("")
  PrintN("-h : hexadecimal output format")
  PrintN("-d : decimal output format")
  PrintN("-b : binary output format")
  PrintN("-hr : hexadecimal output format with 90° clockwise rotation (useful for direct draw routines, 8*8 sprites only)")
  PrintN("-dr : decimal output format with 90° clockwise rotation (useful for direct draw routines, 8*8 sprites only)")
  PrintN("-br : binary output format with 90° clockwise rotation (useful for direct draw routines, 8*8 sprites only)")
  PrintN("-tr : tilesets with 90° clockwise rotation (useful for direct draw routines)")
  PrintN("")
  PrintN("sprite images : input sprite image (*.jpg|*.jpeg|*.png|*.tif|*.tga|*.bmp)")
  Input()
EndIf
End
; IDE Options = PureBasic 5.30 (Windows - x86)
; CursorPosition = 45
; FirstLine = 39
; EnableXP
; EnableUser
; Executable = ..\img2c.exe
; CompileSourceDirectory
; EnablePurifier