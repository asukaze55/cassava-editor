object frOptionBackUp: TfrOptionBackUp
  Left = 0
  Top = 0
  Width = 360
  Height = 400
  TabOrder = 0
  TabStop = True
  object Label1: TLabel
    Left = 24
    Top = 44
    Width = 46
    Height = 13
    Caption = #12501#12449#12452#12523#21517
  end
  object Label2: TLabel
    Left = 24
    Top = 156
    Width = 48
    Height = 13
    Caption = #26178#38291#38291#38548
  end
  object Label4: TLabel
    Left = 168
    Top = 156
    Width = 12
    Height = 13
    Caption = #20998
  end
  object Label3: TLabel
    Left = 24
    Top = 188
    Width = 46
    Height = 13
    Caption = #12501#12449#12452#12523#21517
  end
  object cbBackupOnSave: TCheckBox
    Left = 8
    Top = 16
    Width = 337
    Height = 17
    Caption = #19978#26360#12365#20445#23384#26178#12395#12496#12483#12463#12450#12483#12503
    Checked = True
    State = cbChecked
    TabOrder = 0
    OnClick = cbBackupOnSaveClick
  end
  object edBuFileNameS: TEdit
    Left = 96
    Top = 40
    Width = 185
    Height = 21
    ParentShowHint = False
    ShowHint = False
    TabOrder = 1
    Text = '%f.%x~'
  end
  object cbDelBuSExit: TCheckBox
    Left = 24
    Top = 96
    Width = 281
    Height = 17
    Caption = 'Cassava'#12398#32066#20102#26178#12395#21066#38500#12377#12427
    Enabled = False
    TabOrder = 3
  end
  object cbBackupOnTime: TCheckBox
    Left = 8
    Top = 128
    Width = 337
    Height = 17
    Caption = #23450#26399#30340#12395#12496#12483#12463#12450#12483#12503
    TabOrder = 4
    OnClick = cbBackupOnTimeClick
  end
  object seBuInterval: TEdit
    Left = 94
    Top = 151
    Width = 52
    Height = 21
    TabOrder = 5
    Text = '5'
  end
  object udBuInterval: TUpDown
    Left = 146
    Top = 151
    Width = 16
    Height = 21
    Associate = seBuInterval
    Min = 1
    Max = 1440
    Position = 5
    TabOrder = 6
  end
  object edBuFileNameT: TEdit
    Left = 96
    Top = 184
    Width = 185
    Height = 21
    Enabled = False
    ParentShowHint = False
    ShowHint = False
    TabOrder = 7
    Text = '#%f.%x#'
  end
  object cbBackupOnOpen: TCheckBox
    Left = 24
    Top = 217
    Width = 281
    Height = 17
    Caption = #12501#12449#12452#12523#12434#38283#12356#12383#26178#12395#12377#12368#20316#25104#12377#12427
    Checked = True
    Enabled = False
    State = cbChecked
    TabOrder = 8
  end
  object cbDelBuT: TCheckBox
    Left = 24
    Top = 240
    Width = 281
    Height = 17
    Caption = 'Cassava'#12398#32066#20102#26178#12395#21066#38500#12377#12427
    Checked = True
    Enabled = False
    State = cbChecked
    TabOrder = 9
  end
  object cbDelBuSSaved: TCheckBox
    Left = 24
    Top = 73
    Width = 281
    Height = 17
    Caption = #20445#23384#23436#20102#26178#12395#21066#38500#12377#12427
    Checked = True
    State = cbChecked
    TabOrder = 2
  end
  object Memo1: TMemo
    Left = 8
    Top = 272
    Width = 337
    Height = 125
    TabStop = False
    Color = clBtnFace
    Lines.Strings = (
      #8251#12501#12449#12452#12523#21517#35352#20837#27396#12398#29305#27530#25991#23383
      #12539'%f'#65306#20803#12398#12501#12449#12452#12523#12398#12501#12449#12452#12523#21517#65288#25313#24373#23376#12434#38500#12367#65289
      #12539'%x'#65306#20803#12398#12501#12449#12452#12523#12398#25313#24373#23376
      #12539'%y'#65306#24180#12289'%M'#65306#26376#12289'%d'#65306#26085
      #12539'%H'#65306#26178#12289'%m'#65306#20998#12289'%s'#65306#31186
      #12539'%n'#65306#36899#30058
      #12539#12300'\'#12301#35352#21495#12395#12424#12426#32118#23550#65295#30456#23550#12497#12473#12364#25351#23450#21487#33021)
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 10
  end
end
