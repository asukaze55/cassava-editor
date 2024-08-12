object fmKey: TfmKey
  Left = 247
  Top = 188
  Caption = #12461#12540#12459#12473#12479#12510#12452#12474
  ClientHeight = 400
  ClientWidth = 472
  Color = clBtnFace
  ParentFont = True
  OnShow = FormShow
  TextHeight = 15
  object tvMenu: TTreeView
    Left = 0
    Top = 0
    Width = 249
    Height = 400
    Align = alClient
    HideSelection = False
    Indent = 19
    TabOrder = 0
    OnChange = tvMenuChange
    OnChanging = tvMenuChanging
    Items.NodeData = {
      070100000009540054007200650065004E006F00640065003700000000000000
      00000000FFFFFFFFFFFFFFFFFFFFFFFF000000000000000000010C4300610073
      0073006100760061002000E130CB30E530FC30}
  end
  object Panel1: TPanel
    Left = 249
    Top = 0
    Width = 223
    Height = 400
    Align = alRight
    BevelOuter = bvNone
    TabOrder = 1
    object btnOK: TBitBtn
      Left = 104
      Top = 320
      Width = 113
      Height = 33
      Caption = 'OK'
      Default = True
      ModalResult = 1
      NumGlyphs = 2
      TabOrder = 8
      OnClick = btnOKClick
    end
    object btnCancel: TBitBtn
      Left = 105
      Top = 360
      Width = 112
      Height = 33
      Cancel = True
      Caption = #12461#12515#12531#12475#12523
      ModalResult = 2
      NumGlyphs = 2
      TabOrder = 10
    end
    object cbCtrl: TCheckBox
      Left = 16
      Top = 256
      Width = 57
      Height = 17
      Caption = 'Ctrl'
      TabOrder = 3
    end
    object cbShift: TCheckBox
      Left = 16
      Top = 280
      Width = 57
      Height = 17
      Caption = 'Shift'
      TabOrder = 4
    end
    object rgSCKey: TRadioGroup
      Left = 8
      Top = 32
      Width = 209
      Height = 217
      Caption = #12471#12519#12540#12488#12459#12483#12488#12461#12540
      ItemIndex = 0
      Items.Strings = (
        #12394#12375
        #12450#12523#12501#12449#12505#12483#12488
        #12501#12449#12531#12463#12471#12519#12531
        'Enter'
        'Space'
        'Insert'
        'Delete'
        'Back Space')
      TabOrder = 1
      OnClick = rgSCKeyClick
    end
    object edSCKey: TEdit
      Left = 160
      Top = 72
      Width = 49
      Height = 23
      TabOrder = 2
      OnKeyPress = edSCKeyKeyPress
    end
    object cbAlt: TCheckBox
      Left = 16
      Top = 304
      Width = 57
      Height = 17
      Caption = 'Alt'
      TabOrder = 5
    end
    object StaticText1: TStaticText
      Left = 144
      Top = 98
      Width = 10
      Height = 19
      Alignment = taRightJustify
      Caption = 'F'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Yu Gothic UI'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 9
    end
    object stUseSC: TStaticText
      Left = 176
      Top = 184
      Width = 37
      Height = 37
      Caption = #9734
      Font.Charset = SHIFTJIS_CHARSET
      Font.Color = clBlue
      Font.Height = -32
      Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 11
      Visible = False
    end
    object btnSave: TBitBtn
      Left = 105
      Top = 256
      Width = 113
      Height = 25
      Caption = #20445#23384
      NumGlyphs = 2
      TabOrder = 6
      OnClick = btnSaveClick
    end
    object btnOpen: TBitBtn
      Left = 105
      Top = 280
      Width = 113
      Height = 25
      Caption = #35501#36796
      NumGlyphs = 2
      TabOrder = 7
      OnClick = btnOpenClick
    end
    object edSelected: TEdit
      Left = 8
      Top = 0
      Width = 209
      Height = 23
      TabOrder = 0
      OnChange = edSelectedChange
    end
    object csFNumber: TEdit
      Left = 160
      Top = 96
      Width = 33
      Height = 23
      TabOrder = 12
      Text = '1'
    end
    object udFNumber: TUpDown
      Left = 193
      Top = 96
      Width = 16
      Height = 23
      Associate = csFNumber
      Min = 1
      Max = 24
      Position = 1
      TabOrder = 13
    end
  end
  object dlgOpen: TOpenDialog
    DefaultExt = 'csv'
    Filter = 'CSV'#12487#12540#12479'(*.csv)|*.csv|'#20840#12390#12398#12501#12449#12452#12523'|*.*'
    Title = #12461#12540#35373#23450#35501#12415#36796#12415
    Left = 329
    Top = 256
  end
  object dlgSave: TSaveDialog
    DefaultExt = 'csv'
    Filter = 'CSV'#12487#12540#12479'(*.csv)|*.csv|'#20840#12390#12398#12501#12449#12452#12523'|*.*'
    Title = #12461#12540#35373#23450#20445#23384
    Left = 328
    Top = 288
  end
end
