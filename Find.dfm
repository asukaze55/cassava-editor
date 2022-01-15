object fmFind: TfmFind
  Left = 1252
  Top = 717
  BorderStyle = bsDialog
  Caption = #26908#32034#12539#32622#25563
  ClientHeight = 258
  ClientWidth = 473
  Color = clBtnFace
  ParentFont = True
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  Position = poDefaultSizeOnly
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object btnSearchFromTop: TButton
    Left = 360
    Top = 8
    Width = 105
    Height = 25
    Caption = #20808#38957#12363#12425#26908#32034'(&O)'
    TabOrder = 3
    OnClick = btnSearchFromTopClick
  end
  object btnNext: TButton
    Left = 360
    Top = 40
    Width = 105
    Height = 25
    Caption = #27425#12434#26908#32034
    Default = True
    TabOrder = 4
    OnClick = btnNextClick
  end
  object btnReplace: TButton
    Left = 360
    Top = 72
    Width = 105
    Height = 25
    Caption = #32622#25563#12375#12390#27425#12395'(&X)'
    TabOrder = 5
    OnClick = btnReplaceClick
  end
  object btnAllReplace: TButton
    Left = 360
    Top = 104
    Width = 105
    Height = 25
    Caption = #12377#12409#12390#32622#25563'(&A)'
    TabOrder = 6
    OnClick = btnAllReplaceClick
  end
  object btnCancel: TButton
    Left = 360
    Top = 136
    Width = 105
    Height = 25
    Cancel = True
    Caption = #12461#12515#12531#12475#12523
    TabOrder = 7
    OnClick = btnCancelClick
  end
  object PageControl: TPageControl
    Left = 0
    Top = 0
    Width = 353
    Height = 155
    ActivePage = tsTextSearch
    TabOrder = 0
    OnChange = PageControlChange
    object tsTextSearch: TTabSheet
      Caption = #25991#23383#21015#26908#32034'(&T)'
      object Label1: TLabel
        Left = 8
        Top = 20
        Width = 99
        Height = 13
        Caption = #26908#32034#12377#12427#25991#23383#21015'(&S)'#65306
        FocusControl = edFindText
      end
      object Label2: TLabel
        Left = 8
        Top = 42
        Width = 103
        Height = 13
        Caption = #32622#25563#24460#12398#25991#23383#21015'(&R)'#65306
        FocusControl = edReplaceText
      end
      object edFindText: TEdit
        Left = 120
        Top = 16
        Width = 217
        Height = 21
        TabOrder = 0
      end
      object edReplaceText: TEdit
        Left = 120
        Top = 38
        Width = 217
        Height = 21
        TabOrder = 1
      end
      object cbWordSearch: TCheckBox
        Left = 3
        Top = 84
        Width = 241
        Height = 17
        Caption = #12475#12523#20869#23481#12364#23436#20840#12395#21516#19968#12391#12354#12427#12418#12398#12434#26908#32034'(&W)'
        TabOrder = 3
      end
      object cbRegex: TCheckBox
        Left = 3
        Top = 107
        Width = 241
        Height = 17
        Caption = #27491#35215#34920#29694#26908#32034'(&E)'
        TabOrder = 4
      end
      object cbCase: TCheckBox
        Left = 3
        Top = 61
        Width = 241
        Height = 17
        Caption = #22823#25991#23383#12392#23567#25991#23383#12434#21306#21029'(&C)'
        Checked = True
        State = cbChecked
        TabOrder = 2
      end
    end
    object tsNumSearch: TTabSheet
      Caption = #25968#20516#26908#32034'(&V)'
      ImageIndex = 1
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object Label3: TLabel
        Left = 152
        Top = 52
        Width = 22
        Height = 13
        Caption = #8806'x'#8806
      end
      object Label4: TLabel
        Left = 8
        Top = 24
        Width = 79
        Height = 13
        Caption = #25968#20516#12398#31684#22258'(&R)'#65306
        FocusControl = edMin
      end
      object edMin: TEdit
        Left = 24
        Top = 48
        Width = 121
        Height = 21
        TabOrder = 0
      end
      object edMax: TEdit
        Left = 192
        Top = 48
        Width = 121
        Height = 21
        TabOrder = 1
      end
    end
  end
  object rgRange: TRadioGroup
    Left = 96
    Top = 161
    Width = 257
    Height = 41
    Caption = #26908#32034#23550#35937'(&G)'
    Columns = 3
    ItemIndex = 2
    Items.Strings = (
      #29694#22312#12398#34892
      #29694#22312#12398#21015
      #20840#20307)
    TabOrder = 1
  end
  object rgDirection: TRadioGroup
    Left = 96
    Top = 209
    Width = 177
    Height = 41
    Caption = #26908#32034#26041#21521'(&D)'
    Columns = 2
    ItemIndex = 1
    Items.Strings = (
      #24038#12539#19978#12408
      #21491#12539#19979#12408)
    TabOrder = 2
  end
end
