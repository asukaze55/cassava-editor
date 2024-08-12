object fmPrint: TfmPrint
  Left = 736
  Top = 410
  BorderStyle = bsDialog
  Caption = #21360#21047
  ClientHeight = 270
  ClientWidth = 352
  Color = clBtnFace
  ParentFont = True
  Position = poScreenCenter
  OnClose = FormClose
  OnShow = FormShow
  DesignSize = (
    352
    270)
  TextHeight = 15
  object lblFont: TLabel
    Left = 8
    Top = 244
    Width = 45
    Height = 15
    Caption = 'Font: ???'
  end
  object btnPrint: TButton
    Left = 237
    Top = 8
    Width = 105
    Height = 25
    Anchors = [akTop, akRight]
    Caption = #21360#21047' (&P)...'
    TabOrder = 0
    OnClick = btnPrintClick
  end
  object gbYohaku: TGroupBox
    Left = 8
    Top = 7
    Width = 185
    Height = 88
    Caption = #20313#30333' (mm)'
    TabOrder = 3
    object Label1: TLabel
      Left = 16
      Top = 23
      Width = 26
      Height = 15
      Caption = #24038#65306
    end
    object Label2: TLabel
      Left = 90
      Top = 23
      Width = 26
      Height = 15
      Caption = #21491#65306
    end
    object Label3: TLabel
      Left = 90
      Top = 50
      Width = 26
      Height = 15
      Caption = #19979#65306
    end
    object Label4: TLabel
      Left = 16
      Top = 50
      Width = 26
      Height = 15
      Caption = #19978#65306
    end
    object csYohaku0: TEdit
      Left = 40
      Top = 20
      Width = 28
      Height = 23
      TabOrder = 0
      Text = '15'
    end
    object udYohaku0: TUpDown
      Left = 68
      Top = 20
      Width = 16
      Height = 21
      Associate = csYohaku0
      Max = 1000
      Position = 15
      TabOrder = 1
    end
    object csYohaku1: TEdit
      Left = 114
      Top = 20
      Width = 28
      Height = 23
      TabOrder = 2
      Text = '15'
    end
    object udYohaku1: TUpDown
      Left = 142
      Top = 20
      Width = 16
      Height = 21
      Associate = csYohaku1
      Max = 1000
      Position = 15
      TabOrder = 3
    end
    object csYohaku2: TEdit
      Left = 40
      Top = 47
      Width = 28
      Height = 23
      TabOrder = 4
      Text = '15'
    end
    object udYohaku2: TUpDown
      Left = 68
      Top = 47
      Width = 16
      Height = 21
      Associate = csYohaku2
      Max = 1000
      Position = 15
      TabOrder = 5
    end
    object csYohaku3: TEdit
      Left = 114
      Top = 47
      Width = 28
      Height = 23
      TabOrder = 6
      Text = '15'
    end
    object udYohaku3: TUpDown
      Left = 142
      Top = 47
      Width = 16
      Height = 21
      Associate = csYohaku3
      Max = 1000
      Position = 15
      TabOrder = 7
    end
  end
  object btnFont: TButton
    Left = 237
    Top = 39
    Width = 105
    Height = 25
    Anchors = [akTop, akRight]
    Caption = #12501#12457#12531#12488' (&F)...'
    TabOrder = 1
    OnClick = btnFontClick
  end
  object btnClose: TButton
    Left = 237
    Top = 70
    Width = 105
    Height = 25
    Anchors = [akTop, akRight]
    Cancel = True
    Caption = #38281#12376#12427
    ModalResult = 1
    TabOrder = 2
  end
  object gbHeaderFooter: TGroupBox
    Left = 8
    Top = 101
    Width = 344
    Height = 137
    Caption = #12504#12483#12480#12392#12501#12483#12479
    TabOrder = 4
    object lblHeader: TLabel
      Left = 16
      Top = 24
      Width = 41
      Height = 15
      Caption = #12504#12483#12480#65306
    end
    object Label5: TLabel
      Left = 19
      Top = 51
      Width = 39
      Height = 15
      Caption = #12501#12483#12479#65306
    end
    object edHeader: TEdit
      Left = 54
      Top = 16
      Width = 217
      Height = 23
      TabOrder = 0
    end
    object edFooter: TEdit
      Left = 54
      Top = 43
      Width = 217
      Height = 23
      TabOrder = 2
    end
    object cbHeaderPosition: TComboBox
      Left = 277
      Top = 16
      Width = 48
      Height = 23
      AutoDropDown = True
      Style = csDropDownList
      ItemIndex = 2
      TabOrder = 1
      Text = #20013#22830
      Items.Strings = (
        #12394#12375
        #24038
        #20013#22830
        #21491)
    end
    object cbFooterPosition: TComboBox
      Left = 277
      Top = 43
      Width = 48
      Height = 23
      AutoDropDown = True
      Style = csDropDownList
      ItemIndex = 2
      TabOrder = 3
      Text = #20013#22830
      Items.Strings = (
        #12394#12375
        #24038
        #20013#22830
        #21491)
    end
    object mmHeaderFooterHint: TMemo
      Left = 54
      Top = 70
      Width = 217
      Height = 51
      TabStop = False
      Color = clBtnFace
      Lines.Strings = (
        '%f: '#12501#12449#12452#12523#21517#65288#12497#12473#20184#12365#65289
        '%F: '#12501#12449#12452#12523#21517#65288#12497#12473#12394#12375#65289
        '%p: '#12506#12540#12472#30058#21495)
      ReadOnly = True
      ScrollBars = ssVertical
      TabOrder = 4
    end
  end
  object dlgPrinter: TPrinterSetupDialog
    Left = 280
    Top = 216
  end
  object dlgFont: TFontDialog
    Font.Charset = SHIFTJIS_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
    Font.Style = []
    Left = 312
    Top = 216
  end
end
