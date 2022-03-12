object frOptionDataFormat: TfrOptionDataFormat
  Left = 0
  Top = 0
  Width = 360
  Height = 410
  TabOrder = 0
  TabStop = True
  DesignSize = (
    360
    410)
  object lblName: TLabel
    Left = 8
    Top = 11
    Width = 70
    Height = 13
    Caption = #12487#12540#12479#24418#24335#21517#65306
  end
  object gbExt: TGroupBox
    Left = 0
    Top = 40
    Width = 353
    Height = 97
    Caption = #25313#24373#23376
    TabOrder = 2
    object Label9: TLabel
      Left = 96
      Top = 23
      Width = 66
      Height = 13
      Alignment = taRightJustify
      Caption = #27161#28310#25313#24373#23376#65306
    end
    object Label13: TLabel
      Left = 50
      Top = 47
      Width = 112
      Height = 13
      Alignment = taRightJustify
      Caption = #12371#12398#24418#24335#12391#38283#12367#25313#24373#23376#65306
    end
    object edExts: TEdit
      Left = 168
      Top = 46
      Width = 177
      Height = 21
      Hint = #12300';'#12301#12391#21306#20999#12387#12390#20006#12409#12390#12367#12384#12373#12356#12290
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
    end
    object edDefExt: TEdit
      Left = 168
      Top = 20
      Width = 57
      Height = 21
      Hint = #12489#12483#12488#65288'.'#65289#12399#19981#35201#12290
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      Text = 'csv'
    end
    object cbForceExt: TCheckBox
      Left = 8
      Top = 72
      Width = 329
      Height = 17
      Hint = #26222#36890#12399#12481#12455#12483#12463#12434#12399#12378#12375#12390#12362#12356#12390#12367#12384#12373#12356#12290
      Caption = #12475#12540#12502#26178#12289#24375#21046#30340#12395#27161#28310#12398#25313#24373#23376#12434#20351#29992
      ParentShowHint = False
      ShowHint = True
      TabOrder = 2
    end
  end
  object gbKugiri: TGroupBox
    Left = 0
    Top = 144
    Width = 353
    Height = 97
    Caption = #21306#20999#12426#25991#23383
    TabOrder = 3
    object Label10: TLabel
      Left = 188
      Top = 25
      Width = 86
      Height = 13
      Alignment = taRightJustify
      Caption = #27161#28310#21306#20999#12426#25991#23383#65306
    end
    object Label11: TLabel
      Left = 148
      Top = 47
      Width = 126
      Height = 13
      Alignment = taRightJustify
      Caption = #12525#12540#12489#26178#21306#20999#12426#25991#23383#12522#12473#12488#65306
    end
    object Label12: TLabel
      Left = 79
      Top = 71
      Width = 195
      Height = 13
      Alignment = taRightJustify
      Caption = #12525#12540#12489#26178#21306#20999#12426#25991#23383#12522#12473#12488#65288#36899#32154#12434#28961#35222#65289#65306
    end
    object edDefSepChar: TEdit
      Left = 280
      Top = 20
      Width = 33
      Height = 21
      Hint = #27161#28310#12398#21306#20999#12426#25991#23383#65288#65297#25991#23383#65289
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      Text = ','
    end
    object edSepChars: TEdit
      Left = 280
      Top = 44
      Width = 57
      Height = 21
      Hint = #12371#12398#25991#23383#12391#12418#21306#20999#12425#12428#12414#12377#12290#35079#25968#25991#23383#21487#12290
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
      Text = '\t'
    end
    object edWeakSepChars: TEdit
      Left = 280
      Top = 68
      Width = 57
      Height = 21
      Hint = #12371#12398#25991#23383#12391#12418#21306#20999#12425#12428#12414#12377#12364#12289#36899#32154#12375#12390#20986#12390#12365#12390#12418#65297#12388#12375#12363#21015#12434#36914#12417#12414#12379#12435#12290
      ParentShowHint = False
      ShowHint = True
      TabOrder = 2
      Text = '\_'
    end
  end
  object rgSaveQuote: TRadioGroup
    Left = 0
    Top = 247
    Width = 353
    Height = 105
    Caption = #12463#12458#12540#12488
    ItemIndex = 1
    Items.Strings = (
      #12377#12409#12390#12398#12475#12523#12434'""'#12391#22258#12414#12394#12356
      #24517#35201#12394#12475#12523#12398#12415'""'#12391#22258#12416
      #25991#23383#21015#12399'""'#12391#22258#12416
      #12377#12409#12390#12398#12475#12523#12434'""'#12391#22258#12416)
    TabOrder = 4
  end
  object cbCommaRect: TCheckBox
    Left = 8
    Top = 358
    Width = 321
    Height = 17
    Caption = #12475#12540#12502#26178#12395#21508#34892#12398#21015#25968#12434#32113#19968#12377#12427
    TabOrder = 5
  end
  object cbDummyEOF: TCheckBox
    Left = 8
    Top = 375
    Width = 321
    Height = 25
    Caption = #19979#31471#12539#21491#31471#12398#20869#23481#12398#12394#12356#34892#12539#21015#12418#20445#23384#12377#12427
    TabOrder = 6
  end
  object edName: TEdit
    Left = 84
    Top = 8
    Width = 177
    Height = 21
    TabOrder = 0
    Text = 'edName'
    OnChange = edNameChange
  end
  object btnDeleteType: TButton
    Left = 300
    Top = 6
    Width = 54
    Height = 25
    Anchors = [akTop, akRight]
    Caption = #21066#38500
    TabOrder = 1
    OnClick = btnDeleteTypeClick
  end
end
