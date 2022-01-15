object frOptionFile: TfrOptionFile
  Left = 0
  Top = 0
  Width = 360
  Height = 400
  TabOrder = 0
  TabStop = True
  object gbLock: TGroupBox
    Left = 3
    Top = 85
    Width = 354
    Height = 83
    Caption = #25490#20182#21046#24481
    TabOrder = 1
    object Label1: TLabel
      Left = 12
      Top = 25
      Width = 74
      Height = 13
      Caption = #12501#12449#12452#12523#12398#12525#12483#12463#65306
    end
    object cbCheckTimeStamp: TCheckBox
      Left = 12
      Top = 51
      Width = 338
      Height = 17
      Caption = #12501#12449#12452#12523#12398#12479#12452#12512#12473#12479#12531#12503#12434#12481#12455#12483#12463#12377#12427
      TabOrder = 1
    end
    object cbLockFile: TComboBox
      Left = 136
      Top = 24
      Width = 214
      Height = 21
      Style = csDropDownList
      ItemIndex = 0
      TabOrder = 0
      Text = #12525#12483#12463#12375#12394#12356
      Items.Strings = (
        #12525#12483#12463#12375#12394#12356
        #22793#26356#12375#12383#12501#12449#12452#12523#12434#12525#12483#12463
        #38283#12356#12390#12356#12427#12501#12449#12452#12523#12434#12525#12483#12463)
    end
  end
  object gbKanren: TGroupBox
    Left = 3
    Top = 258
    Width = 354
    Height = 121
    Caption = #38306#36899#20184#12369
    TabOrder = 3
    object cbKanrenCSV: TCheckBox
      Left = 12
      Top = 25
      Width = 338
      Height = 17
      Caption = 'CSV'#12501#12449#12452#12523#12434'Cassava'#12395#38306#36899#20184#12369
      TabOrder = 0
    end
    object cbKanrenCSVNew: TCheckBox
      Left = 12
      Top = 48
      Width = 338
      Height = 17
      Caption = 'CSV'#12434#12456#12463#12473#12503#12525#12540#12521#12398#12300#26032#35215#20316#25104#12301#12395#36861#21152
      TabOrder = 1
    end
    object cbKanrenTSVNew: TCheckBox
      Left = 12
      Top = 94
      Width = 338
      Height = 17
      Caption = 'TSV'#12434#12456#12463#12473#12503#12525#12540#12521#12398#12300#26032#35215#20316#25104#12301#12395#36861#21152
      TabOrder = 3
    end
    object cbKanrenTSV: TCheckBox
      Left = 12
      Top = 71
      Width = 338
      Height = 17
      Caption = 'TSV'#12501#12449#12452#12523#12434'Cassava'#12395#38306#36899#20184#12369
      TabOrder = 2
    end
  end
  object gbCharCode: TGroupBox
    Left = 3
    Top = 174
    Width = 354
    Height = 78
    Caption = #25991#23383#12467#12540#12489
    TabOrder = 2
    object cbCheckKanji: TCheckBox
      Left = 12
      Top = 25
      Width = 338
      Height = 17
      Caption = #12525#12540#12489#26178#12395#25991#23383#12467#12540#12489#12434#21028#21029#12377#12427
      TabOrder = 0
    end
    object cbUnicodeWindowsMapping: TCheckBox
      Left = 12
      Top = 48
      Width = 338
      Height = 17
      Caption = 'Unicode'#22793#25563#12399'Windows'#12510#12483#12500#12531#12464#12523#12540#12523#12395#24467#12358
      TabOrder = 1
    end
  end
  object gbOpen: TGroupBox
    Left = 3
    Top = 3
    Width = 354
    Height = 77
    Caption = #38283#12367
    TabOrder = 0
    object cbNewWindow: TCheckBox
      Left = 13
      Top = 24
      Width = 337
      Height = 17
      Caption = #12501#12449#12452#12523#12434#26032#12375#12356#12454#12451#12531#12489#12454#12391#38283#12367
      TabOrder = 0
    end
    object cbTitleFullPath: TCheckBox
      Left = 13
      Top = 47
      Width = 337
      Height = 17
      Caption = #12501#12449#12452#12523#21517#12434#12501#12523#12497#12473#12391#34920#31034
      TabOrder = 1
    end
  end
end
