object fmFind: TfmFind
  Left = 1252
  Top = 717
  BorderStyle = bsDialog
  Caption = #26908#32034#12539#32622#25563
  ClientHeight = 247
  ClientWidth = 473
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
  Font.Style = []
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  Position = poMainFormCenter
  PixelsPerInch = 96
  TextHeight = 12
  object btnSearchFromTop: TButton
    Left = 360
    Top = 8
    Width = 105
    Height = 25
    Caption = #20808#38957#12363#12425#26908#32034
    TabOrder = 1
    OnClick = btnSearchFromTopClick
  end
  object btnNext: TButton
    Left = 360
    Top = 40
    Width = 105
    Height = 25
    Caption = #27425#12434#26908#32034
    Default = True
    TabOrder = 2
    OnClick = btnNextClick
  end
  object btnReplace: TButton
    Left = 360
    Top = 72
    Width = 105
    Height = 25
    Caption = #32622#25563#12375#12390#27425#12395
    TabOrder = 3
    OnClick = btnReplaceClick
  end
  object btnAllReplace: TButton
    Left = 360
    Top = 104
    Width = 105
    Height = 25
    Caption = #20840#12390#32622#25563
    TabOrder = 4
    OnClick = btnAllReplaceClick
  end
  object btnCancel: TButton
    Left = 360
    Top = 136
    Width = 105
    Height = 25
    Cancel = True
    Caption = #12461#12515#12531#12475#12523
    TabOrder = 5
    OnClick = btnCancelClick
  end
  object PageControl: TPageControl
    Left = 0
    Top = 0
    Width = 353
    Height = 145
    ActivePage = tsTextSearch
    TabIndex = 0
    TabOrder = 0
    OnChange = PageControlChange
    object tsTextSearch: TTabSheet
      Caption = #25991#23383#21015#26908#32034
      object Label1: TLabel
        Left = 8
        Top = 20
        Width = 88
        Height = 12
        Caption = #26908#32034#12377#12427#25991#23383#21015#65306
      end
      object Label2: TLabel
        Left = 8
        Top = 54
        Width = 90
        Height = 12
        Caption = #32622#25563#24460#12398#25991#23383#21015#65306
      end
      object edFindText: TEdit
        Left = 120
        Top = 16
        Width = 217
        Height = 20
        TabOrder = 0
      end
      object edReplaceText: TEdit
        Left = 120
        Top = 50
        Width = 217
        Height = 20
        TabOrder = 1
      end
      object cbWordSearch: TCheckBox
        Left = 8
        Top = 88
        Width = 241
        Height = 17
        Caption = #12527#12540#12489#26908#32034#65288#12475#12523#20840#20307#12392#12398#27604#36611#65289
        TabOrder = 2
      end
    end
    object tsNumSearch: TTabSheet
      Caption = #25968#20516#26908#32034
      ImageIndex = 1
      object Label3: TLabel
        Left = 152
        Top = 52
        Width = 30
        Height = 12
        Caption = #8806'x'#8806
      end
      object Label4: TLabel
        Left = 8
        Top = 24
        Width = 66
        Height = 12
        Caption = #25968#20516#12398#31684#22258#65306
      end
      object edMin: TEdit
        Left = 24
        Top = 48
        Width = 121
        Height = 20
        TabOrder = 0
      end
      object edMax: TEdit
        Left = 192
        Top = 48
        Width = 121
        Height = 20
        TabOrder = 1
      end
    end
  end
  object rgRange: TRadioGroup
    Left = 96
    Top = 152
    Width = 257
    Height = 41
    Caption = #26908#32034#23550#35937
    Columns = 3
    ItemIndex = 2
    Items.Strings = (
      #29694#22312#12398#34892
      #29694#22312#12398#21015
      #20840#20307)
    TabOrder = 6
  end
  object rgDirection: TRadioGroup
    Left = 96
    Top = 200
    Width = 177
    Height = 41
    Caption = #26908#32034#26041#21521
    Columns = 2
    ItemIndex = 1
    Items.Strings = (
      #24038#12539#19978#12408
      #21491#12539#19979#12408)
    TabOrder = 7
  end
end
