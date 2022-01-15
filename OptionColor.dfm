object frOptionColor: TfrOptionColor
  Left = 0
  Top = 0
  Width = 360
  Height = 400
  TabOrder = 0
  TabStop = True
  object gbColor: TGroupBox
    Left = 3
    Top = 4
    Width = 354
    Height = 373
    Caption = #33394
    TabOrder = 0
    object lblFgColor: TLabel
      Left = 27
      Top = 30
      Width = 42
      Height = 13
      Caption = #25991#23383#33394#65306
    end
    object lblBgColor: TLabel
      Left = 27
      Top = 86
      Width = 42
      Height = 13
      Caption = #32972#26223#33394#65306
    end
    object lblFixBgColor: TLabel
      Left = 27
      Top = 142
      Width = 86
      Height = 13
      Caption = #22266#23450#12475#12523#32972#26223#33394#65306
    end
    object lblCalcFgColor: TLabel
      Left = 27
      Top = 254
      Width = 90
      Height = 13
      Caption = #35336#31639#32080#26524#25991#23383#33394#65306
    end
    object lblCalcBgColor: TLabel
      Left = 27
      Top = 282
      Width = 90
      Height = 13
      Caption = #35336#31639#32080#26524#32972#26223#33394#65306
    end
    object lblCalcErrorFgColor: TLabel
      Left = 27
      Top = 310
      Width = 93
      Height = 13
      Caption = #35336#31639#12456#12521#12540#25991#23383#33394#65306
    end
    object lblCalcErrorBgColor: TLabel
      Left = 27
      Top = 338
      Width = 93
      Height = 13
      Caption = #35336#31639#12456#12521#12540#32972#26223#33394#65306
    end
    object lblFixFgColor: TLabel
      Left = 27
      Top = 114
      Width = 86
      Height = 13
      Caption = #22266#23450#12475#12523#25991#23383#33394#65306
    end
    object lblDummyBgColor: TLabel
      Left = 27
      Top = 226
      Width = 88
      Height = 13
      Caption = #12480#12511#12540#12475#12523#32972#26223#33394#65306
    end
    object lblUrlColor: TLabel
      Left = 27
      Top = 58
      Width = 64
      Height = 13
      Caption = 'URL '#25991#23383#33394#65306
    end
    object lblCurrentRowBgColor: TLabel
      Left = 27
      Top = 170
      Width = 92
      Height = 13
      Caption = #12459#12540#12477#12523#34892#32972#26223#33394#65306
    end
    object lblCurrentColBgColor: TLabel
      Left = 27
      Top = 198
      Width = 92
      Height = 13
      Caption = #12459#12540#12477#12523#21015#32972#26223#33394#65306
    end
    object cbFgColor: TColorBox
      Left = 170
      Top = 27
      Width = 145
      Height = 22
      TabOrder = 0
      OnChange = cbFgColorChange
      OnCloseUp = cbFgColorChange
      OnExit = cbFgColorChange
      OnSelect = cbFgColorChange
    end
    object cbBgColor: TColorBox
      Left = 170
      Top = 83
      Width = 145
      Height = 22
      TabOrder = 2
      OnChange = cbBgColorChange
      OnCloseUp = cbBgColorChange
      OnExit = cbBgColorChange
      OnSelect = cbBgColorChange
    end
    object cbFixFgColor: TColorBox
      Left = 170
      Top = 111
      Width = 145
      Height = 22
      TabOrder = 3
    end
    object cbFixBgColor: TColorBox
      Left = 170
      Top = 139
      Width = 145
      Height = 22
      TabOrder = 4
    end
    object cbCalcFgColor: TColorBox
      Left = 170
      Top = 251
      Width = 145
      Height = 22
      TabOrder = 8
    end
    object cbCalcBgColor: TColorBox
      Left = 170
      Top = 279
      Width = 145
      Height = 22
      TabOrder = 9
    end
    object cbCalcErrorFgColor: TColorBox
      Left = 170
      Top = 307
      Width = 145
      Height = 22
      TabOrder = 10
    end
    object cbCalcErrorBgColor: TColorBox
      Left = 170
      Top = 335
      Width = 145
      Height = 22
      TabOrder = 11
    end
    object cbDummyBgColor: TColorBox
      Left = 170
      Top = 223
      Width = 145
      Height = 22
      TabOrder = 7
    end
    object cbUrlColor: TColorBox
      Left = 170
      Top = 55
      Width = 145
      Height = 22
      TabOrder = 1
      OnChange = cbFgColorChange
      OnCloseUp = cbFgColorChange
      OnExit = cbFgColorChange
      OnSelect = cbFgColorChange
    end
    object cbCurrentColBgColor: TColorBox
      Left = 170
      Top = 195
      Width = 145
      Height = 22
      TabOrder = 6
    end
    object cbCurrentRowBgColor: TColorBox
      Left = 170
      Top = 167
      Width = 145
      Height = 22
      TabOrder = 5
    end
  end
end
