object fmSort: TfmSort
  Left = 0
  Top = 0
  Caption = #12477#12540#12488
  ClientHeight = 225
  ClientWidth = 352
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object Label4: TLabel
    Left = 120
    Top = 73
    Width = 12
    Height = 13
    Caption = #65374
  end
  object Label5: TLabel
    Left = 89
    Top = 26
    Width = 99
    Height = 13
    Caption = #21015#30446#12398#12487#12540#12479#12391#12477#12540#12488
    FocusControl = seSortCol
  end
  object Label1: TLabel
    Left = 8
    Top = 50
    Width = 66
    Height = 13
    Caption = #12477#12540#12488#31684#22258'(&R)'
    FocusControl = seSortLeft
  end
  object Label2: TLabel
    Left = 8
    Top = 8
    Width = 34
    Height = 13
    Caption = #12461#12540'(&K)'
    FocusControl = seSortCol
  end
  object seSortLeft: TEdit
    Left = 16
    Top = 69
    Width = 33
    Height = 21
    TabOrder = 2
    Text = '1'
    OnChange = seSortChange
  end
  object udSortLeft: TUpDown
    Left = 49
    Top = 69
    Width = 17
    Height = 21
    Associate = seSortLeft
    ArrowKeys = False
    Min = 1
    Max = 2147483647
    Position = 1
    TabOrder = 3
    Thousands = False
  end
  object seSortTop: TEdit
    Left = 64
    Top = 69
    Width = 33
    Height = 21
    TabOrder = 4
    Text = '1'
    OnChange = seSortChange
  end
  object udSortTop: TUpDown
    Tag = 1
    Left = 97
    Top = 69
    Width = 17
    Height = 21
    Associate = seSortTop
    ArrowKeys = False
    Min = 1
    Max = 2147483647
    Position = 1
    TabOrder = 5
    Thousands = False
  end
  object seSortRight: TEdit
    Left = 138
    Top = 69
    Width = 33
    Height = 21
    TabOrder = 6
    Text = '1'
    OnChange = seSortChange
  end
  object udSortRight: TUpDown
    Tag = 2
    Left = 171
    Top = 69
    Width = 17
    Height = 21
    Associate = seSortRight
    ArrowKeys = False
    Min = 1
    Max = 2147483647
    Position = 1
    TabOrder = 7
    Thousands = False
  end
  object seSortBottom: TEdit
    Left = 186
    Top = 69
    Width = 33
    Height = 21
    TabOrder = 8
    Text = '1'
    OnChange = seSortChange
  end
  object udSortBottom: TUpDown
    Tag = 3
    Left = 219
    Top = 69
    Width = 17
    Height = 21
    Associate = seSortBottom
    ArrowKeys = False
    Min = 1
    Max = 2147483647
    Position = 1
    TabOrder = 9
    Thousands = False
  end
  object seSortCol: TEdit
    Left = 17
    Top = 23
    Width = 49
    Height = 21
    TabOrder = 0
    Text = '1'
  end
  object udSortCol: TUpDown
    Tag = 4
    Left = 66
    Top = 23
    Width = 17
    Height = 21
    Associate = seSortCol
    ArrowKeys = False
    Min = 1
    Max = 32767
    Position = 1
    TabOrder = 1
    Thousands = False
  end
  object btnSort: TButton
    Left = 263
    Top = 8
    Width = 81
    Height = 25
    Caption = #23455#34892'(&S)'
    Default = True
    ModalResult = 1
    TabOrder = 14
    OnClick = btnSortClick
  end
  object btnSortCancel: TButton
    Left = 263
    Top = 39
    Width = 81
    Height = 17
    Cancel = True
    Caption = #12461#12515#12531#12475#12523
    Font.Charset = SHIFTJIS_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS UI Gothic'
    Font.Style = []
    ModalResult = 2
    ParentFont = False
    TabOrder = 15
    OnClick = btnSortCancelClick
  end
  object rgSortDirection: TRadioGroup
    Left = 8
    Top = 99
    Width = 121
    Height = 41
    Caption = #12477#12540#12488#38918'(&D)'
    Columns = 2
    ItemIndex = 0
    Items.Strings = (
      #26119#38918
      #38477#38918)
    TabOrder = 10
  end
  object cbNumSort: TCheckBox
    Left = 8
    Top = 146
    Width = 281
    Height = 17
    Caption = #25968#23383#12398#12475#12523#12399#25968#20516#12398#22823#23567#12391#12477#12540#12488#12377#12427'(&N)'
    Checked = True
    State = cbChecked
    TabOrder = 11
  end
  object cbCaseSensitive: TCheckBox
    Left = 8
    Top = 169
    Width = 281
    Height = 17
    Caption = #22823#25991#23383#12392#23567#25991#23383#12434#21306#21029#12377#12427'(&C)'
    Checked = True
    State = cbChecked
    TabOrder = 12
  end
  object cbZenhanSensitive: TCheckBox
    Left = 8
    Top = 192
    Width = 281
    Height = 17
    Caption = #20840#35282#12392#21322#35282#12434#21306#21029#12377#12427'(&Z)'
    Checked = True
    State = cbChecked
    TabOrder = 13
  end
end
