object frOptionBehavior: TfrOptionBehavior
  Left = 0
  Top = 0
  Width = 360
  Height = 400
  TabOrder = 0
  TabStop = True
  object Label14: TLabel
    Left = 8
    Top = 16
    Width = 129
    Height = 13
    Caption = #22266#23450#12475#12523#12489#12521#12483#12464#26178#12398#21205#20316#65306
  end
  object Label15: TLabel
    Left = 8
    Top = 48
    Width = 116
    Height = 13
    Caption = 'Enter'#12461#12540#12391#12398#12475#12523#31227#21205#65306
  end
  object cbDragMove: TComboBox
    Left = 200
    Top = 12
    Width = 145
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 0
    TabOrder = 0
    Text = #34892#65381#21015#12398#31227#21205
    Items.Strings = (
      #34892#65381#21015#12398#31227#21205
      #35079#25968#34892#12539#21015#36984#25246)
  end
  object cbEnterMove: TComboBox
    Left = 200
    Top = 44
    Width = 145
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 0
    TabOrder = 1
    Text = #31227#21205#12375#12394#12356
    Items.Strings = (
      #31227#21205#12375#12394#12356
      #19979#12408#31227#21205
      #21491#12408#31227#21205)
  end
  object cbLeftArrowInCell: TCheckBox
    Left = 8
    Top = 81
    Width = 337
    Height = 17
    Caption = #24038#12459#12540#12477#12523#12461#12540#12391#12475#12523#20869#12434#31227#21205#12377#12427
    TabOrder = 2
  end
  object cbWheelMoveCursol: TCheckBox
    Left = 8
    Top = 104
    Width = 337
    Height = 17
    Caption = #12510#12454#12473#12507#12452#12540#12523#12391#12459#12540#12477#12523#12434#31227#21205#12377#12427
    TabOrder = 3
  end
  object cbAlwaysShowEditor: TCheckBox
    Left = 8
    Top = 127
    Width = 337
    Height = 17
    Caption = #21336#19968#12475#12523#36984#25246#26178#12399#12377#12368#12395#32232#38598#12514#12540#12489#12395#20837#12427
    TabOrder = 4
  end
  object GroupBox2: TGroupBox
    Left = 4
    Top = 185
    Width = 353
    Height = 81
    Caption = #12510#12463#12525
    TabOrder = 5
    object Label17: TLabel
      Left = 140
      Top = 47
      Width = 12
      Height = 13
      Caption = #22238
    end
    object cbStopMacro: TCheckBox
      Left = 13
      Top = 24
      Width = 332
      Height = 17
      Caption = #25351#23450#22238#25968#12398#12523#12540#12503#12391#12510#12463#12525#12434#24375#21046#32066#20102#12377#12427
      TabOrder = 0
      OnClick = cbStopMacroClick
      OnKeyDown = cbStopMacroKeyUpDown
      OnKeyPress = cbStopMacroKeyPress
      OnKeyUp = cbStopMacroKeyUpDown
    end
    object seStopMacroCount: TEdit
      Left = 13
      Top = 47
      Width = 121
      Height = 21
      TabOrder = 1
      Text = '100000'
    end
  end
  object cbSortAll: TCheckBox
    Left = 8
    Top = 150
    Width = 337
    Height = 17
    Caption = #21336#19968#21015#36984#25246#26178#12399#20840#20307#12434#12477#12540#12488
    TabOrder = 6
  end
end
