object fmReload: TfmReload
  Left = 313
  Top = 244
  BorderStyle = bsDialog
  Caption = '読み込みフォーマット'
  ClientHeight = 151
  ClientWidth = 309
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -16
  Font.Name = 'ＭＳ Ｐゴシック'
  Font.Style = []
  PixelsPerInch = 96
  TextHeight = 16
  object cbDblQuote: TCheckBox
    Left = 8
    Top = 8
    Width = 297
    Height = 17
    Caption = 'ダブルクオーテーション「"」を有効にする'
    State = cbChecked
    TabOrder = 0
  end
  object cbCamma: TCheckBox
    Left = 8
    Top = 32
    Width = 249
    Height = 17
    Caption = 'カンマ「,」を区切り文字とする'
    State = cbChecked
    TabOrder = 1
  end
  object cbTab: TCheckBox
    Left = 8
    Top = 56
    Width = 249
    Height = 17
    Caption = 'タブ文字を区切り文字とする'
    State = cbChecked
    TabOrder = 2
  end
  object cbSpace: TCheckBox
    Left = 8
    Top = 80
    Width = 249
    Height = 17
    Caption = 'スペースを区切り文字とする'
    State = cbChecked
    TabOrder = 3
  end
  object BitBtn1: TBitBtn
    Left = 8
    Top = 120
    Width = 129
    Height = 25
    Caption = '再読み込み'
    TabOrder = 4
    Kind = bkOK
  end
  object BitBtn2: TBitBtn
    Left = 168
    Top = 120
    Width = 129
    Height = 25
    TabOrder = 5
    Kind = bkCancel
  end
end
