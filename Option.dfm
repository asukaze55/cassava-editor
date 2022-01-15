object fmOption: TfmOption
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Cassava '#12458#12503#12471#12519#12531
  ClientHeight = 590
  ClientWidth = 529
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object tvCategory: TTreeView
    Left = 0
    Top = 0
    Width = 145
    Height = 555
    Align = alLeft
    Indent = 20
    ReadOnly = True
    RightClickSelect = True
    TabOrder = 0
    OnChange = tvCategoryChange
    OnMouseUp = tvCategoryMouseUp
  end
  object pnlClient: TPanel
    Left = 145
    Top = 0
    Width = 384
    Height = 555
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 1
    object pnlOption: TPanel
      AlignWithMargins = True
      Left = 5
      Top = 30
      Width = 374
      Height = 520
      Margins.Left = 5
      Margins.Top = 5
      Margins.Right = 5
      Margins.Bottom = 5
      Align = alClient
      BevelOuter = bvNone
      TabOrder = 0
    end
    object pnlHeader: TPanel
      Left = 0
      Top = 0
      Width = 384
      Height = 25
      Align = alTop
      BevelOuter = bvNone
      TabOrder = 1
      DesignSize = (
        384
        25)
      object shapeHeader: TShape
        Left = 6
        Top = 2
        Width = 368
        Height = 25
        Anchors = [akLeft, akTop, akRight]
        Brush.Color = clNavy
        Pen.Style = psClear
        ExplicitWidth = 379
      end
      object lblHeader: TLabel
        Left = 16
        Top = 6
        Width = 37
        Height = 13
        Caption = 'Option'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWhite
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
        Transparent = True
      end
    end
  end
  object pnlBottom: TPanel
    Left = 0
    Top = 555
    Width = 529
    Height = 35
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 2
    DesignSize = (
      529
      35)
    object btnOK: TBitBtn
      Left = 324
      Top = 6
      Width = 97
      Height = 27
      Anchors = [akRight, akBottom]
      Caption = 'OK'
      Default = True
      ModalResult = 1
      NumGlyphs = 2
      TabOrder = 0
      OnClick = btnOKClick
    end
    object btnCancel: TBitBtn
      Left = 427
      Top = 6
      Width = 95
      Height = 27
      Anchors = [akRight, akBottom]
      Cancel = True
      Caption = #12461#12515#12531#12475#12523
      ModalResult = 2
      NumGlyphs = 2
      TabOrder = 1
    end
  end
  object popupMenu: TPopupMenu
    Left = 166
    Top = 38
    object mnApplyToCurrentFile: TMenuItem
      Caption = #32232#38598#20013#12398#12501#12449#12452#12523#12395#36969#29992'(&A)'
      OnClick = mnApplyToCurrentFileClick
    end
    object mnDelete: TMenuItem
      Caption = #21066#38500'(&D)'
      OnClick = mnDeleteClick
    end
  end
end
