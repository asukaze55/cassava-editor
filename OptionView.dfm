object frOptionView: TfrOptionView
  Left = 0
  Top = 0
  Width = 360
  Height = 400
  TabOrder = 0
  object gbFont: TGroupBox
    Left = 3
    Top = 3
    Width = 354
    Height = 137
    Caption = #12501#12457#12531#12488
    TabOrder = 0
    object lblTBMargin: TLabel
      Left = 13
      Top = 53
      Width = 68
      Height = 13
      Caption = #19978#19979#12510#12540#12472#12531#65306
    end
    object lblTBMarginPt: TLabel
      Left = 319
      Top = 53
      Width = 12
      Height = 13
      Caption = 'px'
    end
    object lblLRMargin: TLabel
      Left = 13
      Top = 80
      Width = 68
      Height = 13
      Caption = #24038#21491#12510#12540#12472#12531#65306
    end
    object lblLRMarginPt: TLabel
      Left = 319
      Top = 80
      Width = 12
      Height = 13
      Caption = 'px'
    end
    object lblCellLineMargin: TLabel
      Left = 13
      Top = 109
      Width = 96
      Height = 13
      Caption = #12475#12523#20869#25913#34892#12398#34892#38291#65306
    end
    object lblCellLineMarginPt: TLabel
      Left = 319
      Top = 107
      Width = 12
      Height = 13
      Caption = 'px'
    end
    object btnFont: TButton
      Left = 13
      Top = 19
      Width = 97
      Height = 25
      Caption = #12501#12457#12531#12488#35373#23450
      TabOrder = 0
      OnClick = btnFontClick
    end
    object stFont: TStaticText
      Left = 170
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
    object edTBMargin: TEdit
      Left = 226
      Top = 50
      Width = 73
      Height = 21
      TabOrder = 2
      Text = '0'
    end
    object udTBMargin: TUpDown
      Left = 299
      Top = 50
      Width = 16
      Height = 21
      Associate = edTBMargin
      TabOrder = 3
    end
    object udLRMargin: TUpDown
      Left = 299
      Top = 77
      Width = 16
      Height = 21
      Associate = edLRMargin
      TabOrder = 5
    end
    object edCellLineMargin: TEdit
      Left = 226
      Top = 104
      Width = 73
      Height = 21
      TabOrder = 6
      Text = '0'
    end
    object udCellLineMargin: TUpDown
      Left = 299
      Top = 104
      Width = 16
      Height = 21
      Associate = edCellLineMargin
      TabOrder = 7
    end
    object edLRMargin: TEdit
      Left = 226
      Top = 77
      Width = 73
      Height = 21
      TabOrder = 4
      Text = '0'
    end
  end
  object gbView: TGroupBox
    Left = 3
    Top = 143
    Width = 354
    Height = 146
    Caption = #34920#31034#12458#12503#12471#12519#12531
    TabOrder = 1
    object Label1: TLabel
      Left = 40
      Top = 119
      Width = 90
      Height = 13
      Caption = #23567#25968#28857#20197#19979#26689#25968#65306
    end
    object cbWordWrap: TCheckBox
      Left = 13
      Top = 24
      Width = 337
      Height = 17
      Caption = #38263#12356#12475#12523#12434#25240#12426#36820#12375#12390#34920#31034
      TabOrder = 0
    end
    object cbNumAlignRight: TCheckBox
      Left = 13
      Top = 47
      Width = 337
      Height = 17
      Caption = #25968#20516#12434#21491#23492#12379
      TabOrder = 1
    end
    object cbNum3: TCheckBox
      Left = 13
      Top = 70
      Width = 337
      Height = 17
      Caption = #25968#20516#12434' 3 '#26689#21306#20999#12426#34920#31034
      TabOrder = 2
    end
    object cbOmitDecimal: TCheckBox
      Left = 13
      Top = 93
      Width = 337
      Height = 17
      Caption = #23567#25968#12434#30465#30053#34920#31034
      TabOrder = 3
    end
    object udOmitDigits: TUpDown
      Left = 299
      Top = 116
      Width = 16
      Height = 21
      Associate = edOmitDigits
      Position = 2
      TabOrder = 4
    end
    object edOmitDigits: TEdit
      Left = 226
      Top = 116
      Width = 73
      Height = 21
      TabOrder = 5
      Text = '2'
    end
  end
end
