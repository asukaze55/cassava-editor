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
    ItemIndex = 0
    TabOrder = 1
    Text = #31227#21205#12375#12394#12356
    Items.Strings = (
      #31227#21205#12375#12394#12356
      #19979#12408#31227#21205
      #21491#12408#31227#21205
      #27425#12398#34892#12408#31227#21205
      #12479#12502#12461#12540#12392#36899#21205)
  end
  object cbLeftArrowInCell: TCheckBox
    Left = 8
    Top = 81
    Width = 337
    Height = 17
    Caption = #24038#12459#12540#12477#12523#12461#12540#12391#12475#12523#20869#12434#31227#21205#12377#12427
    TabOrder = 2
  end
  object cbAlwaysShowEditor: TCheckBox
    Left = 8
    Top = 104
    Width = 337
    Height = 17
    Caption = #21336#19968#12475#12523#36984#25246#26178#12399#12377#12368#12395#32232#38598#12514#12540#12489#12395#20837#12427
    TabOrder = 3
  end
  object gbMacro: TGroupBox
    Left = 3
    Top = 264
    Width = 354
    Height = 81
    Caption = #12510#12463#12525
    TabOrder = 7
    object Label17: TLabel
      Left = 141
      Top = 47
      Width = 12
      Height = 13
      Caption = #22238
    end
    object cbStopMacro: TCheckBox
      Left = 14
      Top = 21
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
      Left = 14
      Top = 44
      Width = 121
      Height = 21
      TabOrder = 1
      Text = '100000'
    end
  end
  object cbSortAll: TCheckBox
    Left = 8
    Top = 127
    Width = 337
    Height = 17
    Caption = #21336#19968#21015#36984#25246#26178#12399#20840#20307#12434#12477#12540#12488
    TabOrder = 4
  end
  object cbCalcWidthForAllRow: TCheckBox
    Left = 8
    Top = 150
    Width = 337
    Height = 17
    Caption = #21015#24133#35519#25972#12399#30011#38754#22806#12398#34892#12418#23550#35937#12392#12377#12427
    TabOrder = 5
  end
  object gbWheel: TGroupBox
    Left = 3
    Top = 173
    Width = 354
    Height = 85
    Caption = #12507#12452#12540#12523
    TabOrder = 6
    object Label1: TLabel
      Left = 14
      Top = 46
      Width = 124
      Height = 13
      Caption = #12510#12454#12473#12507#12452#12540#12523#12398#31227#21205#34892#25968
    end
    object Label2: TLabel
      Left = 14
      Top = 22
      Width = 112
      Height = 13
      Caption = #12510#12454#12473#12507#12452#12540#12523#26178#12398#21205#20316
    end
    object edWheelScrollStep: TEdit
      Left = 197
      Top = 44
      Width = 126
      Height = 21
      TabOrder = 1
      Text = '1'
    end
    object udWheelScrollStep: TUpDown
      Left = 323
      Top = 44
      Width = 16
      Height = 21
      Associate = edWheelScrollStep
      Min = 1
      Position = 1
      TabOrder = 2
    end
    object cbWheelMoveCursol: TComboBox
      Left = 197
      Top = 17
      Width = 145
      Height = 21
      Style = csDropDownList
      ItemIndex = 0
      TabOrder = 0
      Text = #30011#38754#12398#12415#12473#12463#12525#12540#12523
      Items.Strings = (
        #30011#38754#12398#12415#12473#12463#12525#12540#12523
        #12459#12540#12477#12523#12434#31227#21205
        #12459#12540#12477#12523#34920#31034#20301#32622#12434#22266#23450)
    end
  end
end
