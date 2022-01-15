object fmLetter: TfmLetter
  Left = 296
  Top = 141
  HelpContext = 1
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = #12399#12364#12365#21360#21047
  ClientHeight = 559
  ClientWidth = 767
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -16
  Font.Name = 'MS UI Gothic'
  Font.Style = []
  OldCreateOrder = True
  Position = poScreenCenter
  OnClose = FormClose
  OnShow = FormShow
  DesignSize = (
    767
    559)
  PixelsPerInch = 96
  TextHeight = 16
  object imPreview: TImage
    Left = 0
    Top = 0
    Width = 377
    Height = 559
    OnClick = UpdatePreviewImage
  end
  object Label4: TLabel
    Left = 647
    Top = 10
    Width = 96
    Height = 16
    Anchors = [akTop, akRight]
    Caption = #32294#26041#21521#24494#35519#25972
    ExplicitLeft = 450
  end
  object Label5: TLabel
    Left = 735
    Top = 35
    Width = 24
    Height = 16
    Anchors = [akTop, akRight]
    Caption = 'mm'
  end
  object Label6: TLabel
    Left = 647
    Top = 60
    Width = 96
    Height = 16
    Anchors = [akTop, akRight]
    Caption = #27178#26041#21521#24494#35519#25972
    ExplicitLeft = 450
  end
  object Label7: TLabel
    Left = 735
    Top = 83
    Width = 24
    Height = 16
    Anchors = [akTop, akRight]
    Caption = 'mm'
  end
  object Label12: TLabel
    Left = 647
    Top = 110
    Width = 74
    Height = 16
    Anchors = [akTop, akRight]
    Caption = #21360#21047#12377#12427#34892
    ExplicitLeft = 450
  end
  object Label13: TLabel
    Left = 693
    Top = 140
    Width = 16
    Height = 16
    Anchors = [akTop, akRight]
    Caption = #65374
    ExplicitLeft = 496
  end
  object lbFont: TLabel
    Left = 390
    Top = 403
    Width = 45
    Height = 16
    Anchors = [akTop, akRight]
    Caption = #12501#12457#12531#12488
  end
  object Label14: TLabel
    Left = 456
    Top = 370
    Width = 223
    Height = 16
    Anchors = [akTop, akRight]
    Caption = #25991#23383#20197#19979#12398#21322#35282#25968#23383#12434#27178#12395#20006#12409#12427
    ExplicitLeft = 259
  end
  object Label18: TLabel
    Left = 671
    Top = 165
    Width = 32
    Height = 16
    Anchors = [akTop, akRight]
    Caption = #37096#25968
    ExplicitLeft = 474
  end
  object btnHelp: TButton
    Left = 685
    Top = 367
    Width = 81
    Height = 31
    Anchors = [akTop, akRight]
    Caption = #12504#12523#12503
    TabOrder = 3
    OnClick = btnHelpClick
  end
  object bxFrom: TGroupBox
    Left = 386
    Top = 0
    Width = 231
    Height = 201
    Anchors = [akTop, akRight]
    Caption = #24046#20986#20154
    TabOrder = 5
    object Label1: TLabel
      Left = 10
      Top = 30
      Width = 64
      Height = 16
      Caption = #37109#20415#30058#21495
    end
    object Label2: TLabel
      Left = 10
      Top = 60
      Width = 32
      Height = 16
      Caption = #20303#25152
    end
    object Label3: TLabel
      Left = 10
      Top = 110
      Width = 32
      Height = 16
      Caption = #21517#21069
    end
    object Label17: TLabel
      Left = 80
      Top = 140
      Width = 126
      Height = 16
      Caption = #34892#30446#12398#12487#12540#12479#12434#20351#29992
    end
    object edMyNumber: TEdit
      Left = 80
      Top = 25
      Width = 71
      Height = 24
      ImeName = 'MS-IME97 '#26085#26412#35486#20837#21147#65404#65405#65411#65425
      TabOrder = 0
      OnChange = UpdatePreviewImage
    end
    object edMyName: TEdit
      Left = 50
      Top = 105
      Width = 111
      Height = 24
      ImeName = 'MS-IME97 '#26085#26412#35486#20837#21147#65404#65405#65411#65425
      TabOrder = 3
      OnChange = UpdatePreviewImage
    end
    object edMyAddress1: TEdit
      Left = 50
      Top = 55
      Width = 171
      Height = 24
      ImeName = 'MS-IME97 '#26085#26412#35486#20837#21147#65404#65405#65411#65425
      TabOrder = 1
      OnChange = UpdatePreviewImage
    end
    object edMyAddress2: TEdit
      Left = 50
      Top = 78
      Width = 171
      Height = 24
      ImeName = 'MS-IME97 '#26085#26412#35486#20837#21147#65404#65405#65411#65425
      TabOrder = 2
      OnChange = UpdatePreviewImage
    end
    object cbMyDataInCsv: TCheckBox
      Left = 10
      Top = 140
      Width = 21
      Height = 17
      TabOrder = 5
      OnClick = UpdatePreviewImage
    end
    object cbNenga: TCheckBox
      Left = 10
      Top = 168
      Width = 209
      Height = 17
      Caption = #24180#36032#29366#12398#20301#32622#12395#21360#21047
      TabOrder = 8
      OnClick = UpdatePreviewImage
    end
    object edMyName2: TEdit
      Left = 160
      Top = 105
      Width = 57
      Height = 24
      ImeName = 'MS-IME97 '#26085#26412#35486#20837#21147#65404#65405#65411#65425
      TabOrder = 4
      OnChange = UpdatePreviewImage
    end
    object edMyDataInCsv: TEdit
      Left = 27
      Top = 135
      Width = 33
      Height = 24
      TabOrder = 6
      Text = '1'
      OnChange = UpdatePreviewImage
    end
    object udMyDataInCsv: TUpDown
      Left = 60
      Top = 135
      Width = 16
      Height = 24
      Associate = edMyDataInCsv
      Min = 1
      Position = 1
      TabOrder = 7
    end
  end
  object bxTo: TGroupBox
    Left = 386
    Top = 212
    Width = 272
    Height = 150
    Anchors = [akTop, akRight]
    Caption = #23451#20808#12487#12540#12479#21015
    TabOrder = 6
    DesignSize = (
      272
      150)
    object Label8: TLabel
      Left = 10
      Top = 30
      Width = 64
      Height = 16
      Caption = #37109#20415#30058#21495
    end
    object Label9: TLabel
      Left = 140
      Top = 30
      Width = 32
      Height = 16
      Caption = #20303#25152
    end
    object Label10: TLabel
      Left = 9
      Top = 60
      Width = 32
      Height = 16
      Caption = #21517#21069
    end
    object Label11: TLabel
      Left = 10
      Top = 92
      Width = 32
      Height = 16
      Caption = #20633#32771
    end
    object Label16: TLabel
      Left = 140
      Top = 60
      Width = 32
      Height = 16
      Caption = #25964#31216
    end
    object cbUseNote: TCheckBox
      Left = 10
      Top = 124
      Width = 71
      Height = 17
      Caption = #20633#32771#12364
      TabOrder = 13
      OnClick = UpdatePreviewImage
    end
    object edUseNote: TEdit
      Left = 80
      Top = 119
      Width = 41
      Height = 24
      ImeName = 'MS-IME97 '#26085#26412#35486#20837#21147#65404#65405#65411#65425
      TabOrder = 14
      Text = '0'
      OnChange = UpdatePreviewImage
    end
    object cbxHowUseNote: TComboBox
      Left = 130
      Top = 119
      Width = 121
      Height = 24
      Style = csDropDownList
      ImeName = 'MS-IME97 '#26085#26412#35486#20837#21147#65404#65405#65411#65425
      ItemIndex = 0
      TabOrder = 15
      Text = #12398#12487#12540#12479#12398#12415
      OnChange = UpdatePreviewImage
      Items.Strings = (
        #12398#12487#12540#12479#12398#12415
        #20197#22806#12398#12487#12540#12479)
    end
    object edPrefix: TEdit
      Left = 178
      Top = 55
      Width = 75
      Height = 24
      ImeName = 'MS-IME97 '#26085#26412#35486#20837#21147#65404#65405#65411#65425
      TabOrder = 10
      Text = ' '#27096
      OnChange = UpdatePreviewImage
    end
    object edToNumber: TEdit
      Left = 80
      Top = 25
      Width = 33
      Height = 24
      TabOrder = 0
      Text = '2'
      OnChange = UpdatePreviewImage
    end
    object udToNumber: TUpDown
      Left = 113
      Top = 25
      Width = 16
      Height = 24
      Associate = edToNumber
      Position = 2
      TabOrder = 1
    end
    object udToAddress2: TUpDown
      Left = 252
      Top = 25
      Width = 16
      Height = 24
      Associate = edToAddress2
      Position = 4
      TabOrder = 5
    end
    object edToAddress2: TEdit
      Left = 224
      Top = 25
      Width = 28
      Height = 24
      TabOrder = 4
      Text = '4'
      OnChange = UpdatePreviewImage
    end
    object udToName2: TUpDown
      Left = 123
      Top = 55
      Width = 16
      Height = 24
      Associate = edToName2
      TabOrder = 9
    end
    object edToName2: TEdit
      Left = 91
      Top = 55
      Width = 32
      Height = 24
      TabOrder = 8
      Text = '0'
      OnChange = UpdatePreviewImage
    end
    object edToNote: TEdit
      Left = 48
      Top = 89
      Width = 28
      Height = 24
      TabOrder = 11
      Text = '5'
      OnChange = UpdatePreviewImage
    end
    object udToNote: TUpDown
      Left = 76
      Top = 89
      Width = 16
      Height = 24
      Associate = edToNote
      Position = 5
      TabOrder = 12
    end
    object edToName: TEdit
      Left = 47
      Top = 55
      Width = 28
      Height = 24
      TabOrder = 6
      Text = '1'
      OnChange = UpdatePreviewImage
    end
    object udToName: TUpDown
      Left = 75
      Top = 55
      Width = 16
      Height = 24
      Associate = edToName
      Position = 1
      TabOrder = 7
    end
    object edToAddress1: TEdit
      Left = 178
      Top = 25
      Width = 28
      Height = 24
      Anchors = [akTop, akRight]
      TabOrder = 2
      Text = '3'
      OnChange = UpdatePreviewImage
    end
    object udToAddress1: TUpDown
      Left = 206
      Top = 25
      Width = 16
      Height = 24
      Anchors = [akTop, akRight]
      Associate = edToAddress1
      Position = 3
      TabOrder = 3
    end
  end
  object btnPrint: TButton
    Left = 685
    Top = 217
    Width = 81
    Height = 62
    Anchors = [akTop, akRight]
    Caption = #21360#21047'...'
    TabOrder = 0
    OnClick = btnPrintClick
  end
  object btnRead: TButton
    Left = 685
    Top = 295
    Width = 81
    Height = 31
    Anchors = [akTop, akRight]
    Caption = #35373#23450#35501#36796
    TabOrder = 1
    OnClick = btnReadClick
  end
  object btnSave: TButton
    Left = 685
    Top = 325
    Width = 81
    Height = 31
    Anchors = [akTop, akRight]
    Caption = #35373#23450#20445#23384
    TabOrder = 2
    OnClick = btnSaveClick
  end
  object btnClose: TButton
    Left = 685
    Top = 397
    Width = 81
    Height = 31
    Anchors = [akTop, akRight]
    Cancel = True
    Caption = #38281#12376#12427
    ModalResult = 1
    TabOrder = 4
  end
  object cbxFont: TComboBox
    Left = 457
    Top = 400
    Width = 222
    Height = 24
    Style = csDropDownList
    Anchors = [akTop, akRight]
    ImeName = 'MS-IME97 '#26085#26412#35486#20837#21147#65404#65405#65411#65425
    TabOrder = 10
    OnChange = UpdatePreviewImage
  end
  object cbHorzNum: TCheckBox
    Left = 390
    Top = 371
    Width = 21
    Height = 17
    Anchors = [akTop, akRight]
    Checked = True
    State = cbChecked
    TabOrder = 7
    OnClick = UpdatePreviewImage
  end
  object edVert: TEdit
    Left = 644
    Top = 32
    Width = 69
    Height = 24
    Anchors = [akTop, akRight]
    TabOrder = 11
    Text = '0'
    OnChange = UpdatePreviewImage
  end
  object udVert: TUpDown
    Left = 713
    Top = 32
    Width = 16
    Height = 24
    Anchors = [akTop, akRight]
    Associate = edVert
    Min = -32768
    Max = 32767
    TabOrder = 12
  end
  object udHorz: TUpDown
    Left = 713
    Top = 80
    Width = 16
    Height = 24
    Anchors = [akTop, akRight]
    Associate = edHorz
    Min = -32768
    Max = 32767
    TabOrder = 14
  end
  object edHorz: TEdit
    Left = 644
    Top = 80
    Width = 69
    Height = 24
    Anchors = [akTop, akRight]
    TabOrder = 13
    Text = '0'
    OnChange = UpdatePreviewImage
  end
  object edTop: TEdit
    Left = 644
    Top = 132
    Width = 33
    Height = 24
    Anchors = [akTop, akRight]
    TabOrder = 15
    Text = '1'
    OnChange = UpdatePreviewImage
  end
  object udTop: TUpDown
    Left = 677
    Top = 132
    Width = 16
    Height = 24
    Anchors = [akTop, akRight]
    Associate = edTop
    Min = 1
    Position = 1
    TabOrder = 16
  end
  object udBottom: TUpDown
    Left = 743
    Top = 132
    Width = 16
    Height = 24
    Anchors = [akTop, akRight]
    Associate = edBottom
    Min = 1
    Position = 1
    TabOrder = 18
  end
  object edBottom: TEdit
    Left = 715
    Top = 132
    Width = 28
    Height = 24
    Anchors = [akTop, akRight]
    TabOrder = 17
    Text = '1'
  end
  object seCopies: TEdit
    Left = 715
    Top = 162
    Width = 28
    Height = 24
    Anchors = [akTop, akRight]
    TabOrder = 19
    Text = '1'
  end
  object udCopies: TUpDown
    Left = 743
    Top = 162
    Width = 16
    Height = 24
    Anchors = [akTop, akRight]
    Associate = seCopies
    Min = 1
    Max = 1000
    Position = 1
    TabOrder = 20
  end
  object udHorzNumMax: TUpDown
    Left = 435
    Top = 368
    Width = 16
    Height = 24
    Anchors = [akTop, akRight]
    Associate = seHorzNumMax
    Min = 1
    Position = 2
    TabOrder = 9
  end
  object seHorzNumMax: TEdit
    Left = 407
    Top = 368
    Width = 28
    Height = 24
    Anchors = [akTop, akRight]
    TabOrder = 8
    Text = '2'
    OnChange = UpdatePreviewImage
  end
  object dlgOpen: TOpenDialog
    Filter = #35373#23450#12487#12540#12479' (*.dat)|*.dat'
  end
  object dlgSave: TSaveDialog
    Filter = #35373#23450#12487#12540#12479' (*.dat)|*.dat'
    Left = 32
  end
  object dlgPrinter: TPrinterSetupDialog
    Left = 64
  end
end
