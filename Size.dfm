object fmSize: TfmSize
  Left = 287
  Top = 315
  BorderStyle = bsDialog
  Caption = #34920#12469#12452#12474#12434#25351#23450#12375#12390#26032#35215#20316#25104
  ClientHeight = 98
  ClientWidth = 246
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -16
  Font.Name = 'MS UI Gothic'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  PixelsPerInch = 96
  TextHeight = 16
  object Label1: TLabel
    Left = 14
    Top = 18
    Width = 40
    Height = 16
    Caption = #34892#25968#65306
  end
  object Label2: TLabel
    Left = 122
    Top = 18
    Width = 40
    Height = 16
    Caption = #21015#25968#65306
  end
  object btnCancel: TBitBtn
    Left = 128
    Top = 56
    Width = 105
    Height = 33
    TabOrder = 0
    Kind = bkCancel
  end
  object btnOK: TBitBtn
    Left = 5
    Top = 56
    Width = 108
    Height = 33
    Caption = #20316#25104
    TabOrder = 1
    Kind = bkOK
  end
  object seHeight: TEdit
    Left = 60
    Top = 15
    Width = 36
    Height = 24
    TabOrder = 2
    Text = '3'
  end
  object udHeight: TUpDown
    Left = 96
    Top = 15
    Width = 16
    Height = 24
    Associate = seHeight
    Min = 1
    Max = 32767
    Position = 3
    TabOrder = 3
  end
  object seWidth: TEdit
    Left = 168
    Top = 15
    Width = 36
    Height = 24
    TabOrder = 4
    Text = '3'
  end
  object udWidth: TUpDown
    Left = 204
    Top = 15
    Width = 16
    Height = 24
    Associate = seWidth
    Min = 1
    Max = 32767
    Position = 3
    TabOrder = 5
  end
end
