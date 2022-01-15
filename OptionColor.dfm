object frOptionColor: TfrOptionColor
  Left = 0
  Top = 0
  Width = 360
  Height = 400
  TabOrder = 0
  TabStop = True
  object lblFgColor: TLabel
    Left = 27
    Top = 118
    Width = 42
    Height = 13
    Caption = #25991#23383#33394#65306
  end
  object lblBgColor: TLabel
    Left = 27
    Top = 174
    Width = 42
    Height = 13
    Caption = #32972#26223#33394#65306
  end
  object lblFixBgColor: TLabel
    Left = 27
    Top = 230
    Width = 86
    Height = 13
    Caption = #22266#23450#12475#12523#32972#26223#33394#65306
  end
  object lblCalcFgColor: TLabel
    Left = 27
    Top = 286
    Width = 90
    Height = 13
    Caption = #35336#31639#32080#26524#25991#23383#33394#65306
  end
  object lblCalcBgColor: TLabel
    Left = 27
    Top = 314
    Width = 90
    Height = 13
    Caption = #35336#31639#32080#26524#32972#26223#33394#65306
  end
  object lblCalcErrorFgColor: TLabel
    Left = 27
    Top = 342
    Width = 93
    Height = 13
    Caption = #35336#31639#12456#12521#12540#25991#23383#33394#65306
  end
  object lblCalcErrorBgColor: TLabel
    Left = 27
    Top = 370
    Width = 93
    Height = 13
    Caption = #35336#31639#12456#12521#12540#32972#26223#33394#65306
  end
  object lblFixFgColor: TLabel
    Left = 27
    Top = 202
    Width = 86
    Height = 13
    Caption = #22266#23450#12475#12523#25991#23383#33394#65306
  end
  object lblDummyBgColor: TLabel
    Left = 27
    Top = 258
    Width = 88
    Height = 13
    Caption = #12480#12511#12540#12475#12523#32972#26223#33394#65306
  end
  object lblLineMargin: TLabel
    Left = 27
    Top = 58
    Width = 30
    Height = 13
    Caption = #34892#38291#65306
  end
  object lblLineMarginPt: TLabel
    Left = 319
    Top = 58
    Width = 12
    Height = 13
    Caption = 'px'
  end
  object lblUrlColor: TLabel
    Left = 27
    Top = 146
    Width = 64
    Height = 13
    Caption = 'URL '#25991#23383#33394#65306
  end
  object btnFont: TButton
    Left = 27
    Top = 19
    Width = 97
    Height = 25
    Caption = #12501#12457#12531#12488#35373#23450
    TabOrder = 0
    OnClick = btnFontClick
  end
  object stFont: TStaticText
    Left = 146
    Top = 19
    Width = 75
    Height = 17
    Alignment = taCenter
    BorderStyle = sbsSunken
    Caption = 'Aa'#12354#12353#12450#12449#20124#23431
    Color = clWhite
    ParentColor = False
    TabOrder = 1
  end
  object cbFgColor: TColorBox
    Left = 170
    Top = 115
    Width = 145
    Height = 22
    ItemHeight = 16
    TabOrder = 5
    OnChange = cbFgColorChange
    OnCloseUp = cbFgColorChange
    OnExit = cbFgColorChange
    OnSelect = cbFgColorChange
  end
  object cbBgColor: TColorBox
    Left = 170
    Top = 171
    Width = 145
    Height = 22
    ItemHeight = 16
    TabOrder = 7
    OnChange = cbBgColorChange
    OnCloseUp = cbBgColorChange
    OnExit = cbBgColorChange
    OnSelect = cbBgColorChange
  end
  object cbFixFgColor: TColorBox
    Left = 170
    Top = 199
    Width = 145
    Height = 22
    ItemHeight = 16
    TabOrder = 8
  end
  object cbFixBgColor: TColorBox
    Left = 170
    Top = 227
    Width = 145
    Height = 22
    ItemHeight = 16
    TabOrder = 9
  end
  object cbCalcFgColor: TColorBox
    Left = 170
    Top = 283
    Width = 145
    Height = 22
    ItemHeight = 16
    TabOrder = 11
  end
  object cbCalcBgColor: TColorBox
    Left = 170
    Top = 311
    Width = 145
    Height = 22
    ItemHeight = 16
    TabOrder = 12
  end
  object cbCalcErrorFgColor: TColorBox
    Left = 170
    Top = 339
    Width = 145
    Height = 22
    ItemHeight = 16
    TabOrder = 13
  end
  object cbCalcErrorBgColor: TColorBox
    Left = 170
    Top = 367
    Width = 145
    Height = 22
    ItemHeight = 16
    TabOrder = 14
  end
  object cbDummyBgColor: TColorBox
    Left = 170
    Top = 255
    Width = 145
    Height = 22
    ItemHeight = 16
    TabOrder = 10
  end
  object edLineMargin: TEdit
    Left = 170
    Top = 55
    Width = 127
    Height = 21
    TabOrder = 2
    Text = '0'
  end
  object udLineMargin: TUpDown
    Left = 297
    Top = 55
    Width = 16
    Height = 21
    Associate = edLineMargin
    TabOrder = 3
  end
  object cbNoMarginInCellLines: TCheckBox
    Left = 27
    Top = 82
    Width = 286
    Height = 17
    Caption = #12475#12523#20869#25913#34892#12391#12399#34892#38291#12434#12354#12369#12394#12356
    TabOrder = 4
  end
  object cbUrlColor: TColorBox
    Left = 170
    Top = 143
    Width = 145
    Height = 22
    ItemHeight = 16
    TabOrder = 6
    OnChange = cbFgColorChange
    OnCloseUp = cbFgColorChange
    OnExit = cbFgColorChange
    OnSelect = cbFgColorChange
  end
end
