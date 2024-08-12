object fmPasteDialog: TfmPasteDialog
  Left = 308
  Top = 199
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = #36028#12426#20184#12369#12458#12503#12471#12519#12531
  ClientHeight = 289
  ClientWidth = 390
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -16
  Font.Name = 'Yu Gothic UI'
  Font.Style = []
  Position = poScreenCenter
  TextHeight = 21
  object Label1: TLabel
    Left = 0
    Top = 0
    Width = 237
    Height = 21
    Caption = #36984#25246#38936#22495#12392#12487#12540#12479#12398#12469#12452#12474#12364#36949#12356#12414#12377#12290
  end
  object Label2: TLabel
    Left = 0
    Top = 50
    Width = 245
    Height = 21
    Caption = #19979#12398#26041#27861#12391#36028#12426#20184#12369#12390#12424#12429#12375#12356#12391#12377#12363#65311
  end
  object lbMessage: TLabel
    Left = 20
    Top = 20
    Width = 4
    Height = 21
  end
  object Way: TRadioGroup
    Left = 20
    Top = 80
    Width = 351
    Height = 161
    Caption = #36028#12426#20184#12369#26041#27861
    ItemIndex = 2
    Items.Strings = (
      #36984#25246#38936#22495#12392#37325#12394#12387#12383#37096#20998#12398#12415#12395#36028#12426#20184#12369
      #36984#25246#38936#22495#12395#12367#12426#36820#12375#20966#29702#12434#12375#12390#36028#12426#20184#12369
      #12487#12540#12479#12398#12469#12452#12474#12391#19978#26360#12365
      #29694#22312#12398#20869#23481#12434#21491#12395#31227#21205#12373#12379#12390#12487#12540#12479#12434#25407#20837
      #29694#22312#12398#20869#23481#12434#19979#12395#31227#21205#12373#12379#12390#12487#12540#12479#12434#25407#20837
      #12486#12461#12473#12488#12392#12375#12390'1'#12475#12523#20869#12395#36028#12426#20184#12369)
    TabOrder = 0
  end
  object btnOK: TBitBtn
    Left = 30
    Top = 252
    Width = 161
    Height = 31
    Kind = bkOK
    NumGlyphs = 2
    TabOrder = 1
  end
  object btnCancel: TBitBtn
    Left = 200
    Top = 252
    Width = 161
    Height = 31
    Kind = bkCancel
    NumGlyphs = 2
    TabOrder = 2
  end
end
