object fmSaveFormat: TfmSaveFormat
  Left = 299
  Top = 264
  Width = 410
  Height = 309
  Caption = 'fmSaveFormat'
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -16
  Font.Name = '�l�r �o�S�V�b�N'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 16
  object pcSaveFormat: TPageControl
    Left = 0
    Top = 0
    Width = 402
    Height = 282
    ActivePage = HTML
    Align = alClient
    TabOrder = 0
    object TabSheet1: TTabSheet
      Caption = 'CSV'
      object Label1: TLabel
        Left = 16
        Top = 16
        Width = 60
        Height = 16
        Caption = '�s��؂�'
      end
      object Label2: TLabel
        Left = 16
        Top = 48
        Width = 60
        Height = 16
        Caption = '���؂�'
      end
      object edRowSep: TEdit
        Left = 88
        Top = 8
        Width = 121
        Height = 24
        TabOrder = 0
        Text = '\n'
      end
      object Edit1: TEdit
        Left = 88
        Top = 40
        Width = 121
        Height = 24
        TabOrder = 1
        Text = ','
      end
    end
    object HTML: TTabSheet
      Caption = 'HTML'
      ImageIndex = 1
      object Label3: TLabel
        Left = 16
        Top = 88
        Width = 48
        Height = 16
        Caption = '��J�n'
      end
      object Label4: TLabel
        Left = 16
        Top = 56
        Width = 48
        Height = 16
        Caption = '�s�J�n'
      end
      object Label5: TLabel
        Left = 144
        Top = 88
        Width = 48
        Height = 16
        Caption = '��I��'
      end
      object Label6: TLabel
        Left = 168
        Top = 56
        Width = 48
        Height = 16
        Caption = '�s�I��'
      end
      object Label7: TLabel
        Left = 16
        Top = 24
        Width = 42
        Height = 16
        Caption = '�w�b�_'
      end
      object Label8: TLabel
        Left = 16
        Top = 120
        Width = 37
        Height = 16
        Caption = '�t�b�^'
      end
      object Edit2: TEdit
        Left = 88
        Top = 80
        Width = 33
        Height = 24
        TabOrder = 0
        Text = ','
      end
      object Edit3: TEdit
        Left = 88
        Top = 48
        Width = 65
        Height = 24
        TabOrder = 1
        Text = '<tr>\n'
      end
      object Edit4: TEdit
        Left = 216
        Top = 48
        Width = 33
        Height = 24
        TabOrder = 2
        Text = '\n'
      end
      object Edit5: TEdit
        Left = 216
        Top = 80
        Width = 33
        Height = 24
        TabOrder = 3
        Text = ','
      end
      object Edit6: TEdit
        Left = 88
        Top = 16
        Width = 289
        Height = 24
        TabOrder = 4
        Text = '<html>\n<body>\n<table>'
      end
      object Edit7: TEdit
        Left = 88
        Top = 112
        Width = 289
        Height = 24
        TabOrder = 5
        Text = '</table>\n</body>\n</html>'
      end
    end
  end
end
