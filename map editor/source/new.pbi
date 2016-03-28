#TitleFrame_x = 1
#TitleFrame_y = 1
#TitleFrame_w = 202
#TitleFrame_h = 38
#TitleInput_x = #TitleFrame_x+5
#TitleInput_y = #TitleFrame_y+15
#TitleInput_w = #TitleFrame_w-10
#TitleInput_h = 20
#DimensionsFrame_x = #TitleFrame_x
#DimensionsFrame_y = #TitleFrame_y+#TitleFrame_h+1
#DimensionsFrame_w = #TitleFrame_w
#DimensionsFrame_h = #TitleFrame_h
#TextWidth_x = #TitleFrame_x+5
#TextWidth_y = #DimensionsFrame_y+17
#TextWidth_w = 42
#TextWidth_h = 13
#SpinWidth_x = #TextWidth_x+#TextWidth_w+3
#SpinWidth_y = #TextWidth_y-3
#SpinWidth_w = 40
#SpinWidth_h = 20
#TextHeight_x = #SpinWidth_x+#SpinWidth_w+5
#TextHeight_y = #TextWidth_y
#TextHeight_w = 42
#TextHeight_h = 13
#SpinHeight_x = #TextHeight_x+#TextHeight_w+3
#SpinHeight_y = #TextHeight_y-3
#SpinHeight_w = 40
#SpinHeight_h = 20
#OKButton_x = #DimensionsFrame_x
#OKButton_y = #DimensionsFrame_y+#DimensionsFrame_h+2
#OKButton_w = #DimensionsFrame_w/2-1
#OKButton_h = 24
#CancelButton_x = #OKButton_x+#OKButton_w+1
#CancelButton_y = #OKButton_y
#CancelButton_w = #OKButton_w
#CancelButton_h = #OKButton_h
#Window_new_w = #TitleFrame_w+2
#Window_new_h = #TitleFrame_h+2+#DimensionsFrame_h+2+#OKButton_h

Enumeration 1000
  #TitleFrame
  #TitleInput
  #DimensionsFrame
  #TextWidth
  #SpinWidth
  #TextHeight
  #SpinHeight
  #OKButton
  #CancelButton
EndEnumeration

Declare New()
; IDE Options = PureBasic 5.11 (Windows - x86)
; CursorPosition = 37
; FirstLine = 12
; UseMainFile = main.pb
; CurrentDirectory = ..\
; EnablePurifier