object frOptionView: TfrOptionView
  Left = 0
  Top = 0
  Width = 360
  Height = 510
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
      NumbersOnly = True
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
      NumbersOnly = True
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
      NumbersOnly = True
      TabOrder = 4
      Text = '0'
    end
  end
  object gbView: TGroupBox
    Left = 3
    Top = 146
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
      NumbersOnly = True
      TabOrder = 5
      Text = '2'
    end
  end
  object gbTooltip: TGroupBox
    Left = 3
    Top = 403
    Width = 354
    Height = 105
    Caption = #12484#12540#12523#12481#12483#12503
    TabOrder = 3
    object lblHintPause: TLabel
      Left = 13
      Top = 52
      Width = 196
      Height = 13
      Caption = #12509#12483#12503#12450#12483#12503#34920#31034#12414#12391#12398#24453#27231#26178#38291#65288#12511#12522#31186#65289#65306
    end
    object lblHintHidePause: TLabel
      Left = 13
      Top = 77
      Width = 143
      Height = 13
      Caption = #12509#12483#12503#12450#12483#12503#34920#31034#26178#38291#65288#12511#12522#31186#65289#65306
    end
    object lblHintPauseMs: TLabel
      Left = 321
      Top = 52
      Width = 13
      Height = 13
      Caption = 'ms'
    end
    object lblHintHidePauseMs: TLabel
      Left = 321
      Top = 77
      Width = 13
      Height = 13
      Caption = 'ms'
    end
    object cbShowToolTipForLongCell: TCheckBox
      Left = 13
      Top = 24
      Width = 337
      Height = 17
      Caption = #38263#12356#12475#12523#12398#20869#23481#12434#12509#12483#12503#12450#12483#12503#34920#31034#12377#12427
      Checked = True
      State = cbChecked
      TabOrder = 0
    end
    object edHintHidePause: TEdit
      Left = 226
      Top = 74
      Width = 73
      Height = 21
      NumbersOnly = True
      TabOrder = 3
      Text = '500'
    end
    object edHintPause: TEdit
      Left = 226
      Top = 47
      Width = 73
      Height = 21
      NumbersOnly = True
      TabOrder = 1
      Text = '2,500'
    end
    object udHintPause: TUpDown
      Left = 299
      Top = 47
      Width = 16
      Height = 21
      Associate = edHintPause
      Max = 2147483647
      Position = 2500
      TabOrder = 2
    end
    object udHintHidePause: TUpDown
      Left = 299
      Top = 74
      Width = 16
      Height = 21
      Associate = edHintHidePause
      Max = 2147483647
      Position = 500
      TabOrder = 4
    end
  end
  object gbRefresh: TGroupBox
    Left = 3
    Top = 298
    Width = 354
    Height = 99
    Caption = #21015#24133#12392#34892#12398#39640#12373
    TabOrder = 2
    object lblMaxRowHeightLines: TLabel
      Left = 14
      Top = 70
      Width = 181
      Height = 13
      Caption = #12475#12523#20869#25913#34892#12434#21547#12416#34892#12398#39640#12373#12398#26368#22823#20516#65306
    end
    object lblMaxRowHeightLinesUnit: TLabel
      Left = 321
      Top = 70
      Width = 12
      Height = 13
      Caption = #34892
    end
    object cbCompactColWidth: TCheckBox
      Left = 14
      Top = 21
      Width = 337
      Height = 17
      Caption = #21015#25968#12364#22810#12356#22580#21512#12399#21015#24133#12434#29421#12367#12377#12427
      TabOrder = 0
    end
    object edMaxRowHeightLines: TEdit
      Left = 226
      Top = 67
      Width = 89
      Height = 21
      TabOrder = 2
      Text = '1.5'
    end
    object cbCalcWidthForAllRow: TCheckBox
      Left = 14
      Top = 44
      Width = 337
      Height = 17
      Caption = #30011#38754#22806#12398#34892#12418#21015#24133#35519#25972#12398#23550#35937#12392#12377#12427
      TabOrder = 1
    end
  end
end
