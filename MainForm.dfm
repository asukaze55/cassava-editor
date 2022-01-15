object fmMain: TfmMain
  Left = 302
  Top = 229
  Caption = 'Cassava Editor'
  ClientHeight = 561
  ClientWidth = 585
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Menu = Menu
  OldCreateOrder = True
  ShowHint = True
  OnAfterMonitorDpiChanged = FormAfterMonitorDpiChanged
  OnCloseQuery = FormCloseQuery
  PixelsPerInch = 96
  TextHeight = 13
  object StatusBar: TStatusBar
    Left = 0
    Top = 542
    Width = 585
    Height = 19
    Panels = <
      item
        Width = 200
      end>
  end
  object MainPanel: TPanel
    Left = 0
    Top = 28
    Width = 585
    Height = 514
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 1
    object pnlSearch: TPanel
      Left = 0
      Top = 482
      Width = 585
      Height = 32
      Align = alBottom
      TabOrder = 0
      Visible = False
      DesignSize = (
        585
        32)
      object btnNext: TSpeedButton
        Left = 274
        Top = 5
        Width = 77
        Height = 22
        Caption = #27425'(&N)'
        Flat = True
        Glyph.Data = {
          F6000000424DF600000000000000760000002800000010000000100000000100
          04000000000080000000130B0000130B0000100000000000000084020400FC8E
          8C00FCFEFC002300000084A79C00EE00ED00120012000000000024D8BA000942
          0100B42F0000000000001000C800F000ED00122A120000000000222222222222
          2222222222200222222222222200002222222222200110022222222200111100
          2222222001111110022222000001100000222222220110222222222222011022
          2222222222011022222222222201102222222222220110222222222222011022
          2222222222011022222222222200002222222222222222222222}
        OnClick = btnNextClick
      end
      object btnPrevious: TSpeedButton
        Left = 357
        Top = 4
        Width = 77
        Height = 22
        Caption = #21069'(&P)'
        Flat = True
        Glyph.Data = {
          F6000000424DF600000000000000760000002800000010000000100000000100
          04000000000080000000130B0000130B0000100000000000000084020400FC8E
          8C00FCFEFC002300000084A79C00EE00ED00120012000000000024D8BA000942
          0100B42F0000000000001000C800F000ED00122A120000000000222222222222
          2222222222000022222222222201102222222222220110222222222222011022
          2222222222011022222222222201102222222222220110222222222222011022
          2222220000011000002222200111111002222222001111002222222220011002
          2222222222000022222222222220022222222222222222222222}
        OnClick = btnPreviousClick
      end
      object Label1: TLabel
        Left = 37
        Top = 10
        Width = 30
        Height = 13
        Caption = #26908#32034#65306
      end
      object btnSearchOption: TSpeedButton
        Left = 490
        Top = 5
        Width = 93
        Height = 22
        Anchors = [akTop, akRight]
        Caption = #12458#12503#12471#12519#12531'(&O)'
        Flat = True
        OnClick = btnSearchOptionClick
        ExplicitLeft = 488
      end
      object btnSearchCancel: TSpeedButton
        Left = 8
        Top = 5
        Width = 23
        Height = 22
        Hint = #26908#32034#12497#12493#12523#12434#38281#12376#12427
        Caption = #215
        Flat = True
        ParentShowHint = False
        ShowHint = True
        OnClick = btnSearchCancelClick
      end
      object edFindText: TEdit
        Left = 83
        Top = 5
        Width = 185
        Height = 21
        TabOrder = 0
        OnKeyDown = edFindTextKeyDown
      end
    end
  end
  object CoolBar: TControlBar
    Left = 0
    Top = 0
    Width = 585
    Height = 28
    Align = alTop
    AutoSize = True
    BevelEdges = [beTop]
    TabOrder = 2
    OnResize = CoolBarResize
    object tbarAdditional: TToolBar
      Left = 246
      Top = 2
      Width = 309
      Height = 22
      AutoSize = True
      Caption = 'tbarAdditional'
      Images = imlAdditional
      TabOrder = 1
      Wrapable = False
      object tsbSort: TToolButton
        Left = 0
        Top = 0
        Hint = #12477#12540#12488
        AutoSize = True
        Caption = 'tsbSort'
        ImageIndex = 0
        ParentShowHint = False
        ShowHint = True
        OnClick = sbSortClick
      end
      object ToolButton12: TToolButton
        Left = 23
        Top = 0
        Width = 8
        Caption = 'ToolButton12'
        ImageIndex = 1
        Style = tbsSeparator
      end
      object tsbInsRow: TToolButton
        Left = 31
        Top = 0
        Hint = #65297#34892#25407#20837
        AutoSize = True
        Caption = 'tsbInsRow'
        ImageIndex = 1
        ParentShowHint = False
        ShowHint = True
        OnClick = mnInsRowClick
      end
      object tsbInsCol: TToolButton
        Left = 54
        Top = 0
        Hint = #65297#21015#25407#20837
        AutoSize = True
        Caption = 'tsbInsCol'
        ImageIndex = 2
        ParentShowHint = False
        ShowHint = True
        OnClick = mnInsColClick
      end
      object tsbCutRow: TToolButton
        Left = 77
        Top = 0
        Hint = #65297#34892#21066#38500
        AutoSize = True
        Caption = 'tsbCutRow'
        ImageIndex = 3
        ParentShowHint = False
        ShowHint = True
        OnClick = mnCutRowClick
      end
      object tsbCutCol: TToolButton
        Left = 100
        Top = 0
        Hint = #65297#21015#21066#38500
        AutoSize = True
        Caption = 'tsbCutCol'
        ImageIndex = 4
        ParentShowHint = False
        ShowHint = True
        OnClick = mnCutColClick
      end
      object ToolButton17: TToolButton
        Left = 123
        Top = 0
        Width = 8
        Caption = 'ToolButton17'
        ImageIndex = 5
        Style = tbsSeparator
      end
      object tsbFind: TToolButton
        Left = 131
        Top = 0
        Hint = #26908#32034#12539#32622#25563
        AutoSize = True
        Caption = 'tsbFind'
        ImageIndex = 5
        ParentShowHint = False
        ShowHint = True
        OnClick = sbFindClick
      end
      object ToolButton19: TToolButton
        Left = 154
        Top = 0
        Width = 8
        Caption = 'ToolButton19'
        ImageIndex = 6
        Style = tbsSeparator
      end
      object tsbRefresh: TToolButton
        Left = 162
        Top = 0
        Hint = #34920#31034#12398#26356#26032
        AutoSize = True
        Caption = 'tsbRefresh'
        ImageIndex = 6
        ParentShowHint = False
        ShowHint = True
        OnClick = mnRefreshClick
      end
      object tsbFont: TToolButton
        Left = 185
        Top = 0
        Hint = #12501#12457#12531#12488
        AutoSize = True
        Caption = 'tsbFont'
        ImageIndex = 8
        ParentShowHint = False
        ShowHint = True
        OnClick = mnFontClick
      end
      object sbCalcExpression: TToolButton
        Left = 208
        Top = 0
        Hint = #12475#12523#20869#35336#31639#24335#12434#20966#29702
        AllowAllUp = True
        AutoSize = True
        Caption = 'sbCalcExpression'
        ImageIndex = 12
        ParentShowHint = False
        ShowHint = True
        Style = tbsCheck
        OnClick = mnCalcExpressionClick
      end
      object ToolButton23: TToolButton
        Left = 231
        Top = 0
        Width = 8
        Caption = 'ToolButton23'
        ImageIndex = 9
        Style = tbsSeparator
      end
      object tsbFixUpLeft: TToolButton
        Left = 239
        Top = 0
        Hint = #12459#12540#12477#12523#20301#32622#12414#12391#12434#22266#23450
        AutoSize = True
        Caption = 'tsbFixUpLeft'
        ImageIndex = 9
        ParentShowHint = False
        ShowHint = True
        OnClick = mnFixUpLeftClick
      end
      object tsbFixFirstRow: TToolButton
        Left = 262
        Top = 0
        Hint = #65297#34892#30446#12434#22266#23450
        AutoSize = True
        Caption = 'tsbFixFirstRow'
        ImageIndex = 10
        ParentShowHint = False
        ShowHint = True
        Style = tbsCheck
        OnClick = mnFixFirstRowClick
      end
      object tsbFixFirstCol: TToolButton
        Left = 285
        Top = 0
        Hint = #65297#21015#30446#12434#22266#23450
        AutoSize = True
        Caption = 'tsbFixFirstCol'
        ImageIndex = 11
        ParentShowHint = False
        ShowHint = True
        Style = tbsCheck
        OnClick = mnFixFirstColClick
      end
    end
    object tbarNormal: TToolBar
      Left = 11
      Top = 2
      Width = 222
      Height = 22
      AutoSize = True
      Caption = 'tbarNormal'
      DisabledImages = imlNormalDisabled
      Images = imlNormal
      TabOrder = 0
      Wrapable = False
      object tsbNew: TToolButton
        Left = 0
        Top = 0
        Hint = #26032#35215#20316#25104
        AutoSize = True
        ImageIndex = 6
        ParentShowHint = False
        ShowHint = True
        OnClick = mnNewClick
      end
      object tsbOpen: TToolButton
        Left = 23
        Top = 0
        Hint = #38283#12367
        AutoSize = True
        DropdownMenu = PopMenuOpen
        ImageIndex = 7
        ParentShowHint = False
        ShowHint = True
        Style = tbsDropDown
        OnClick = mnOpenClick
      end
      object tsbSave: TToolButton
        Left = 67
        Top = 0
        Hint = #19978#26360#12365#20445#23384
        Action = acSave
        AutoSize = True
        ParentShowHint = False
        ShowHint = True
      end
      object ToolButton4: TToolButton
        Left = 90
        Top = 0
        Width = 8
        Caption = 'ToolButton4'
        ImageIndex = 0
        Style = tbsSeparator
      end
      object tsbCut: TToolButton
        Left = 98
        Top = 0
        Hint = #20999#12426#21462#12426
        Action = acCut
        AutoSize = True
        ParentShowHint = False
        ShowHint = True
      end
      object tsbCopy: TToolButton
        Left = 121
        Top = 0
        Hint = #12467#12500#12540
        Action = acCopy
        AutoSize = True
        ParentShowHint = False
        ShowHint = True
      end
      object tsbPaste: TToolButton
        Left = 144
        Top = 0
        Hint = #36028#12426#20184#12369
        Action = acPaste
        AutoSize = True
        ParentShowHint = False
        ShowHint = True
      end
      object ToolButton8: TToolButton
        Left = 167
        Top = 0
        Width = 8
        Caption = 'ToolButton8'
        ImageIndex = 0
        Style = tbsSeparator
      end
      object tsbUndo: TToolButton
        Left = 175
        Top = 0
        Hint = #20803#12395#25147#12377
        Action = acUndo
        AutoSize = True
        ParentShowHint = False
        ShowHint = True
      end
      object tsbRedo: TToolButton
        Left = 198
        Top = 0
        Hint = #12420#12426#30452#12375
        Action = acRedo
        AutoSize = True
        ParentShowHint = False
        ShowHint = True
      end
    end
  end
  object Menu: TMainMenu
    Left = 290
    Top = 32
    object mnFile: TMenuItem
      Caption = #12501#12449#12452#12523'(&F)'
      object mnNew: TMenuItem
        Caption = #26032#35215#20316#25104'(&N)'
        OnClick = mnNewClick
      end
      object mnOpen: TMenuItem
        Caption = #38283#12367'(&O)...'
        OnClick = mnOpenClick
      end
      object mnReload: TMenuItem
        Caption = #20877#35501#12415#36796#12415'(&R)'
        OnClick = mnReloadClick
      end
      object mnOpenHistory: TMenuItem
        Caption = #26368#36817#20351#29992#12375#12383#12501#12449#12452#12523'(&H)'
        object mnOpenHistory0: TMenuItem
          Caption = '&0: ('#12394#12375')'
          Enabled = False
          OnClick = mnOpenHistorysClick
        end
        object mnOpenHistory1: TMenuItem
          Tag = 1
          Caption = '&1: ('#12394#12375')'
          Visible = False
          OnClick = mnOpenHistorysClick
        end
        object mnOpenHistory2: TMenuItem
          Tag = 2
          Caption = '&2: ('#12394#12375')'
          Visible = False
          OnClick = mnOpenHistorysClick
        end
        object mnOpenHistory3: TMenuItem
          Tag = 3
          Caption = '&3: ('#12394#12375')'
          Visible = False
          OnClick = mnOpenHistorysClick
        end
        object mnOpenHistory4: TMenuItem
          Tag = 4
          Caption = '&4: ('#12394#12375')'
          Visible = False
          OnClick = mnOpenHistorysClick
        end
        object mnOpenHistory5: TMenuItem
          Tag = 5
          Caption = '&5: ('#12394#12375')'
          Visible = False
          OnClick = mnOpenHistorysClick
        end
        object mnOpenHistory6: TMenuItem
          Tag = 6
          Caption = '&6: ('#12394#12375')'
          Visible = False
          OnClick = mnOpenHistorysClick
        end
        object mnOpenHistory7: TMenuItem
          Tag = 7
          Caption = '&7: ('#12394#12375')'
          Visible = False
          OnClick = mnOpenHistorysClick
        end
        object mnOpenHistory8: TMenuItem
          Tag = 8
          Caption = '&8: ('#12394#12375')'
          Visible = False
          OnClick = mnOpenHistorysClick
        end
        object mnOpenHistory9: TMenuItem
          Tag = 9
          Caption = '&9: ('#12394#12375')'
          Visible = False
          OnClick = mnOpenHistorysClick
        end
        object N8: TMenuItem
          Caption = '-'
        end
        object mnOpenHistoryClear: TMenuItem
          Caption = #23653#27508#12434#12463#12522#12450'(&C)'
          OnClick = mnOpenHistoryClearClick
        end
      end
      object mnReloadCode: TMenuItem
        Caption = #25991#23383#12467#12540#12489#25351#23450#20877#35501#12415#36796#12415'(&D)'
        Enabled = False
        OnClick = mnCharCodeClick
        object mnReloadCodeShiftJIS: TMenuItem
          Caption = '&Shift-JIS'
          GroupIndex = 1
          RadioItem = True
          OnClick = mnReloadCodeClick
        end
        object mnReloadCodeEUC: TMenuItem
          Tag = 1
          Caption = '&EUC'
          GroupIndex = 1
          RadioItem = True
          OnClick = mnReloadCodeClick
        end
        object mnReloadCodeJIS: TMenuItem
          Tag = 2
          Caption = '&JIS'
          GroupIndex = 1
          RadioItem = True
          OnClick = mnReloadCodeClick
        end
        object mnReloadCodeUTF8: TMenuItem
          Tag = 3
          Caption = 'UTF-&8'
          GroupIndex = 1
          RadioItem = True
          OnClick = mnReloadCodeClick
        end
        object mnReloadCodeUnicode: TMenuItem
          Tag = 4
          Caption = '&UTF-16LE'
          GroupIndex = 1
          RadioItem = True
          OnClick = mnReloadCodeClick
        end
        object mnReloadCodeUTF16BE: TMenuItem
          Tag = 5
          Caption = 'UTF-16&BE'
          GroupIndex = 1
          RadioItem = True
          OnClick = mnReloadCodeClick
        end
      end
      object mnN10: TMenuItem
        Caption = '-'
      end
      object mnSave: TMenuItem
        Action = acSave
        ShortCut = 16467
      end
      object mnSaveAs: TMenuItem
        Caption = #21517#21069#12434#20184#12369#12390#20445#23384'(&A)...'
        OnClick = mnSaveAsClick
      end
      object mnExport: TMenuItem
        Caption = #12456#12463#12473#12509#12540#12488'(&T)...'
        OnClick = mnExportClick
      end
      object mnCharCode: TMenuItem
        Caption = #20445#23384#26178#25991#23383#12467#12540#12489'(&C)'
        OnClick = mnCharCodeClick
        object mnSjis: TMenuItem
          Caption = '&Shift-JIS'
          Checked = True
          GroupIndex = 1
          RadioItem = True
          OnClick = mnKCodeClick
        end
        object mnEuc: TMenuItem
          Tag = 1
          Caption = '&EUC'
          GroupIndex = 1
          RadioItem = True
          OnClick = mnKCodeClick
        end
        object mnJis: TMenuItem
          Tag = 2
          Caption = '&JIS'
          GroupIndex = 1
          RadioItem = True
          OnClick = mnKCodeClick
        end
        object mnUtf8: TMenuItem
          Tag = 3
          Caption = 'UTF-&8'
          GroupIndex = 1
          RadioItem = True
          OnClick = mnKCodeClick
        end
        object mnUnicode: TMenuItem
          Tag = 4
          Caption = '&UTF-16LE'
          GroupIndex = 1
          RadioItem = True
          OnClick = mnKCodeClick
        end
        object mnUtf16be: TMenuItem
          Tag = 5
          Caption = 'UTF-16&BE'
          GroupIndex = 1
          RadioItem = True
          OnClick = mnKCodeClick
        end
        object mnN13: TMenuItem
          Caption = '-'
          GroupIndex = 1
        end
        object mnLfcr: TMenuItem
          Caption = 'CR+LF'
          Checked = True
          GroupIndex = 2
          RadioItem = True
          OnClick = mnReturnCodeClick
        end
        object mnLf: TMenuItem
          Tag = 10
          Caption = 'LF'
          GroupIndex = 2
          RadioItem = True
          OnClick = mnReturnCodeClick
        end
        object mnCr: TMenuItem
          Tag = 13
          Caption = 'CR'
          GroupIndex = 2
          RadioItem = True
          OnClick = mnReturnCodeClick
        end
      end
      object mnN1: TMenuItem
        Caption = '-'
      end
      object mnOpenCellFile: TMenuItem
        Caption = #29694#22312#12398#12475#12523#12398#12501#12449#12452#12523#12434#38283#12367'(&E)'
        ShortCut = 116
        OnClick = mnOpenCellFileClick
      end
      object mnPrint: TMenuItem
        Caption = #21360#21047'(&P)...'
        OnClick = mnPrintClick
      end
      object mnLetterPrint: TMenuItem
        Caption = #12399#12364#12365#21360#21047'(&L)...'
        OnClick = mnLetterPrintClick
      end
      object mnN2: TMenuItem
        Caption = '-'
      end
      object mnEnd: TMenuItem
        Caption = #32066#20102'(&X)'
        OnClick = mnEndClick
      end
    end
    object mnEdit: TMenuItem
      Caption = #32232#38598'(&E)'
      object mnUndo: TMenuItem
        Action = acUndo
        ShortCut = 16474
      end
      object mnRedo: TMenuItem
        Action = acRedo
        ShortCut = 16473
      end
      object mnN3: TMenuItem
        Caption = '-'
      end
      object mnCut: TMenuItem
        Action = acCut
        ShortCut = 16472
      end
      object mnCopy: TMenuItem
        Action = acCopy
        ShortCut = 16451
      end
      object mnPaste: TMenuItem
        Action = acPaste
        ShortCut = 16470
      end
      object mnSelectAll: TMenuItem
        Caption = #12377#12409#12390#36984#25246'(&A)'
        ShortCut = 16449
        OnClick = mnSelectAllClick
      end
      object mnN9: TMenuItem
        Caption = '-'
      end
      object mnCutFormat: TMenuItem
        Caption = #24418#24335#12434#25351#23450#12375#12390#20999#12426#21462#12426'(&D)'
        object mnCutFormatDefault: TMenuItem
          Caption = 'Default'
          OnClick = mnCutFormatDefaultClick
        end
      end
      object mnCopyFormat: TMenuItem
        Caption = #24418#24335#12434#25351#23450#12375#12390#12467#12500#12540'(&M)'
        object mnCopyFormatDefault: TMenuItem
          Caption = 'Default'
          OnClick = mnCopyFormatDefaultClick
        end
      end
      object mnPasteFormat: TMenuItem
        Caption = #24418#24335#12434#25351#23450#12375#12390#36028#12426#20184#12369'(&F)'
        object mnPasteFormatDefault: TMenuItem
          Caption = 'Default'
          OnClick = mnPasteFormatDefaultClick
        end
      end
      object mnPasteOption: TMenuItem
        Caption = #36028#12426#20184#12369#12458#12503#12471#12519#12531'(&O)'
        object mnPasteNormal: TMenuItem
          Tag = -1
          Caption = #27161#28310'(&N)'
          Checked = True
          RadioItem = True
          OnClick = mnPasteOptionsClick
        end
        object mnN7: TMenuItem
          Caption = '-'
        end
        object mnPasteOption0: TMenuItem
          Caption = #36984#25246#38936#22495#12392#37325#12394#12387#12383#37096#20998#12398#12415#12395#36028#12426#20184#12369'(&S)'
          RadioItem = True
          OnClick = mnPasteOptionsClick
        end
        object mnPasteOption1: TMenuItem
          Tag = 1
          Caption = #36984#25246#38936#22495#12395#12367#12426#36820#12375#20966#29702#12434#12375#12390#36028#12426#20184#12369'(&P)'
          RadioItem = True
          OnClick = mnPasteOptionsClick
        end
        object mnPasteOption2: TMenuItem
          Tag = 2
          Caption = #12487#12540#12479#12398#12469#12452#12474#12391#19978#26360#12365'(&O)'
          RadioItem = True
          OnClick = mnPasteOptionsClick
        end
        object mnPasteOption3: TMenuItem
          Tag = 3
          Caption = #20869#23481#12434#21491#12395#31227#21205#12373#12379#12390#12487#12540#12479#12434#25407#20837'(&R)'
          RadioItem = True
          OnClick = mnPasteOptionsClick
        end
        object mnPasteOption4: TMenuItem
          Tag = 4
          Caption = #20869#23481#12434#19979#12395#31227#21205#12373#12379#12390#12487#12540#12479#12434#25407#20837'(&D)'
          RadioItem = True
          OnClick = mnPasteOptionsClick
        end
        object mnPasteOption5: TMenuItem
          Tag = 5
          Caption = #12486#12461#12473#12488#12392#12375#12390'1'#12475#12523#20869#12395#36028#12426#20184#12369'(&1)'
          RadioItem = True
          OnClick = mnPasteOptionsClick
        end
      end
      object mnN6: TMenuItem
        Caption = '-'
      end
      object mnSort: TMenuItem
        Caption = #12477#12540#12488'(&Q)...'
        OnClick = mnSortClick
      end
      object mnN12: TMenuItem
        Caption = '-'
      end
      object mnSelectRow: TMenuItem
        Caption = #34892#36984#25246'(&R)'
        ShortCut = 8224
        OnClick = mnSelectRowClick
      end
      object mnSelectCol: TMenuItem
        Caption = #21015#36984#25246'(&L)'
        ShortCut = 16416
        OnClick = mnSelectColClick
      end
      object mnN8: TMenuItem
        Caption = '-'
      end
      object mnCopySum: TMenuItem
        Caption = #21512#35336#12434#12467#12500#12540'(&S)'
        OnClick = mnCopySumClick
      end
      object mnCopyAvr: TMenuItem
        Caption = #24179#22343#12434#12467#12500#12540'(&V)'
        OnClick = mnCopyAvrClick
      end
      object mnTransChar: TMenuItem
        Caption = #25991#23383#22793#25563'(&H)'
        object mnTransChar0: TMenuItem
          Caption = #33521#25968#12539#35352#21495#12434#21322#35282#12395#22793#25563'(&1)'
          OnClick = mnTransCharClick
        end
        object mnTransChar1: TMenuItem
          Tag = 1
          Caption = #33521#25968#12539#35352#21495#12434#20840#35282#12395#22793#25563'(&2)'
          OnClick = mnTransCharClick
        end
        object N1: TMenuItem
          Caption = '-'
        end
        object mnTransChar2: TMenuItem
          Tag = 2
          Caption = #33521#23383#12434#22823#25991#23383#12395#22793#25563'(&C)'
          OnClick = mnTransCharClick
        end
        object mnTransChar3: TMenuItem
          Tag = 3
          Caption = #33521#23383#12434#23567#25991#23383#12395#22793#25563'(&S)'
          OnClick = mnTransCharClick
        end
        object N3: TMenuItem
          Caption = '-'
        end
        object mnTransChar4: TMenuItem
          Tag = 4
          Caption = #12459#12490#12434#21322#35282#12395#22793#25563'(&H)'
          OnClick = mnTransKanaClick
        end
        object mnTransChar5: TMenuItem
          Tag = 5
          Caption = #12459#12490#12434#20840#35282#12395#22793#25563'(&F)'
          OnClick = mnTransKanaClick
        end
      end
      object mnSequence: TMenuItem
        Caption = #36899#32154#12487#12540#12479'(&W)'
        OnClick = mnSequenceClick
        object mnSequenceS: TMenuItem
          Caption = #36899#30058#20316#25104'(&S)'
          OnClick = mnSequenceSClick
        end
        object mnSequenceC: TMenuItem
          Caption = #65297#34892#30446#12434#12467#12500#12540'(&C)'
          OnClick = mnSequenceCClick
        end
      end
    end
    object mnTable: TMenuItem
      Caption = #25407#20837#12539#21066#38500'(&T)'
      object mnInsRow: TMenuItem
        Caption = #34892#25407#20837'(&R)'
        ShortCut = 8205
        OnClick = mnInsRowClick
      end
      object mnInsCol: TMenuItem
        Caption = #21015#25407#20837'(&C)'
        OnClick = mnInsColClick
      end
      object mnCutRow: TMenuItem
        Caption = #34892#21066#38500'(&D)'
        OnClick = mnCutRowClick
      end
      object mnCutCol: TMenuItem
        Caption = #21015#21066#38500'(&X)'
        OnClick = mnCutColClick
      end
      object mnEnter: TMenuItem
        Caption = #34892#20998#21106'(&E)'
        ShortCut = 16397
        OnClick = mnEnterClick
      end
      object mnNewLine: TMenuItem
        Caption = #12475#12523#20869#25913#34892'(&L)'
        ShortCut = 32781
        OnClick = mnNewLineClick
      end
      object mnN4: TMenuItem
        Caption = '-'
      end
      object mnConnectCell: TMenuItem
        Caption = #12475#12523#32080#21512'(&N)'
        ShortCut = 16392
        OnClick = mnConnectCellClick
      end
      object mnInsertCell: TMenuItem
        Caption = #12475#12523#25407#20837'(&I)'
        object mnInsertCellRight: TMenuItem
          Caption = #21491#21521#12365'(&R)'
          ShortCut = 16429
          OnClick = mnInsertCellRightClick
        end
        object mnInsertCellDown: TMenuItem
          Caption = #19979#21521#12365'(&D)'
          ShortCut = 24621
          OnClick = mnInsertCellDownClick
        end
      end
      object mnDeleteCell: TMenuItem
        Caption = #12475#12523#21066#38500'(&T)'
        object mnDeleteCellLeft: TMenuItem
          Caption = #24038#12395#12388#12417#12427'(&L)'
          ShortCut = 16430
          OnClick = mnDeleteCellLeftClick
        end
        object mnDeleteCellUp: TMenuItem
          Caption = #19978#12395#12388#12417#12427'(&U)'
          ShortCut = 24622
          OnClick = mnDeleteCellUpClick
        end
      end
    end
    object mnSearch: TMenuItem
      Caption = #26908#32034'(&S)'
      object mnQuickFind: TMenuItem
        Caption = #31777#26131#26908#32034'(&Q)...'
        ShortCut = 16454
        OnClick = mnQuickFindClick
      end
      object mnFind: TMenuItem
        Caption = #26908#32034#12539#32622#25563'(&F)...'
        OnClick = mnFindClick
      end
      object mnFindNext: TMenuItem
        Caption = #27425#12434#26908#32034'(&S)'
        ShortCut = 114
        OnClick = mnFindNextClick
      end
      object mnFindBack: TMenuItem
        Caption = #21069#12434#26908#32034'(&B)'
        ShortCut = 8306
        OnClick = mnFindBackClick
      end
    end
    object mnView: TMenuItem
      Caption = #34920#31034'(&V)'
      object mnRefresh: TMenuItem
        Caption = #34920#31034#12398#26356#26032'(&R)'
        ShortCut = 16604
        OnClick = mnRefreshClick
      end
      object mnN15: TMenuItem
        Caption = '-'
      end
      object mnFont: TMenuItem
        Caption = #12501#12457#12531#12488'(&F)...'
        OnClick = mnFontClick
      end
      object mnStayOnTop: TMenuItem
        Caption = #24120#12395#26368#21069#38754#12395#34920#31034'(&S)'
        OnClick = mnStayOnTopClick
      end
      object mnN16: TMenuItem
        Caption = '-'
      end
      object mnCalcExpression: TMenuItem
        Caption = #12475#12523#20869#35336#31639#24335#12434#20966#29702'(&E)'
        OnClick = mnCalcExpressionClick
      end
      object mnN14: TMenuItem
        Caption = '-'
      end
      object mnShowToolbar: TMenuItem
        Caption = #12484#12540#12523#12496#12540'(&T)'
        Checked = True
        OnClick = mnShowToolbarClick
      end
      object mnShowStatusbar: TMenuItem
        Caption = #12473#12486#12540#12479#12473#12496#12540'(&B)'
        Checked = True
        OnClick = mnShowStatusbarClick
      end
      object mnN5: TMenuItem
        Caption = '-'
      end
      object mnFixFirstRow: TMenuItem
        Caption = #65297#34892#30446#12434#22266#23450'(&W)'
        OnClick = mnFixFirstRowClick
      end
      object mnFixFirstCol: TMenuItem
        Caption = #65297#21015#30446#12434#22266#23450'(&C)'
        OnClick = mnFixFirstColClick
      end
      object mnFixUpLeft: TMenuItem
        Caption = #12459#12540#12477#12523#20301#32622#12414#12391#12434#22266#23450'(&X)'
        OnClick = mnFixUpLeftClick
      end
      object mnUnFix: TMenuItem
        Caption = #22266#23450#12434#35299#38500'(&U)'
        OnClick = mnUnFixClick
      end
    end
    object mnMacro: TMenuItem
      Caption = #12510#12463#12525'(&M)'
      OnClick = mnMacroClick
      object mnMacroOpenUserFolder: TMenuItem
        Caption = #12518#12540#12470#12540#12510#12463#12525#12501#12457#12523#12480#12434#38283#12367'(&U)'
        OnClick = mnMacroOpenUserFolderClick
      end
      object mnMacroOpenFolder: TMenuItem
        Caption = #12450#12503#12522#12465#12540#12471#12519#12531#12510#12463#12525#12501#12457#12523#12480#12434#38283#12367'(&F)'
        OnClick = mnMacroOpenFolderClick
      end
      object mnMacroExecute: TMenuItem
        Caption = #25351#23450#12375#12383#12510#12463#12525#12434#23455#34892'(&X)'
        OnClick = mnMacroExecuteClick
      end
      object mnMacroTerminate: TMenuItem
        Action = acMacroTerminate
      end
      object N2: TMenuItem
        Caption = '-'
      end
    end
    object mnOption: TMenuItem
      Caption = #12458#12503#12471#12519#12531'(&O)'
      object mnOptionDlg: TMenuItem
        Caption = #12458#12503#12471#12519#12531'(&O)...'
        ShortCut = 16463
        OnClick = mnOptionDlgClick
      end
      object mnKey: TMenuItem
        Caption = #12461#12540#12459#12473#12479#12510#12452#12474'(&K)...'
        OnClick = mnKeyClick
      end
      object mnAppli: TMenuItem
        Caption = #22806#37096#12450#12503#12522#12465#12540#12471#12519#12531'(&A)'
        object mnAppli0: TMenuItem
          Tag = 1
          Caption = '&0: '#26410#35373#23450
          Enabled = False
          OnClick = mnAppliClick
        end
        object mnAppli1: TMenuItem
          Tag = 1
          Caption = '&1: '#26410#35373#23450
          Enabled = False
          OnClick = mnAppliClick
        end
        object mnAppli2: TMenuItem
          Tag = 1
          Caption = '&2: '#26410#35373#23450
          Enabled = False
          OnClick = mnAppliClick
        end
      end
    end
    object mnHelp: TMenuItem
      Caption = #12504#12523#12503'(&H)'
      object mnContents: TMenuItem
        Caption = #12504#12523#12503'(&H)...'
        ShortCut = 112
        OnClick = mnHelpContentsClick
      end
      object mnCheckUpdate: TMenuItem
        Caption = #26356#26032#12398#30906#35469'(&C)...'
        OnClick = mnCheckUpdateClick
      end
      object mnAbout: TMenuItem
        Caption = #12496#12540#12472#12519#12531#24773#22577'(&A)...'
        OnClick = mnAboutClick
      end
    end
  end
  object dlgOpen: TOpenDialog
    Filter = 
      'Cassava (*.csv;*.txt;*.tsv)|*.csv;*.txt;*.tsv|CSV (*.csv)|*.csv|' +
      'TSV (*.tsv)|*.tsv|'#12377#12409#12390#12398#12501#12449#12452#12523' (*.*)|*.*'
    Left = 450
    Top = 32
  end
  object dlgSave: TSaveDialog
    Filter = 'Default (*.csv)|*.csv|CSV (*.csv)|*.csv|TSV (*.tsv)|*.tsv|'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableSizing]
    Left = 514
    Top = 32
  end
  object PopMenu: TPopupMenu
    OnPopup = PopMenuPopup
    Left = 322
    Top = 32
    object mnpCut: TMenuItem
      Action = acCut
    end
    object mnpCopy: TMenuItem
      Action = acCopy
    end
    object mnpPaste: TMenuItem
      Action = acPaste
    end
    object mnpPasteInsert: TMenuItem
      Caption = #25407#20837#36028#12426#20184#12369'(&P)'
      Visible = False
      OnClick = mnpPasteInsertClick
    end
    object mnpSelectAll: TMenuItem
      Caption = #12377#12409#12390#36984#25246'(&A)'
      OnClick = mnSelectAllClick
    end
    object mnpCutFormat: TMenuItem
      Caption = #24418#24335#12434#36984#25246#12375#12390#20999#12426#21462#12426'(&D)'
    end
    object mnpCopyFormat: TMenuItem
      Caption = #24418#24335#12434#36984#25246#12375#12390#12467#12500#12540'(&M)'
    end
    object mnpPasteFormat: TMenuItem
      Caption = #24418#24335#12434#36984#25246#12375#12390#36028#12426#20184#12369'(&F)'
    end
    object mnpKugiri: TMenuItem
      Caption = '-'
      Visible = False
    end
    object mnpInsRow: TMenuItem
      Caption = #34892#25407#20837'(&R)'
      Visible = False
      OnClick = mnInsRowClick
    end
    object mnpInsCol: TMenuItem
      Caption = #21015#25407#20837'(&C)'
      Visible = False
      OnClick = mnInsColClick
    end
    object mnpCutRow: TMenuItem
      Caption = #34892#21066#38500'(&D)'
      Visible = False
      OnClick = mnCutRowClick
    end
    object mnpCutCol: TMenuItem
      Caption = #21015#21066#38500'(&X)'
      Visible = False
      OnClick = mnCutColClick
    end
    object mnpKugiri2: TMenuItem
      Caption = '-'
    end
    object mnpSort: TMenuItem
      Caption = #12371#12398#21015#12391#12477#12540#12488'(&Q)'
      Visible = False
      OnClick = mnpSortClick
    end
    object mnpNarrow: TMenuItem
      Caption = #24133#12434#29421#12367#12377#12427'(&N)'
      Visible = False
      OnClick = mnpNarrowClick
    end
    object mnpDefWidth: TMenuItem
      Caption = #27161#28310#12398#24133#12395#12377#12427'(&W)'
      Visible = False
      OnClick = mnpDefWidthClick
    end
  end
  object dlgFont: TFontDialog
    Font.Charset = SHIFTJIS_CHARSET
    Font.Color = clBlack
    Font.Height = -16
    Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
    Font.Style = []
    Options = [fdAnsiOnly, fdEffects]
    Left = 546
    Top = 32
  end
  object tmAutoSaver: TTimer
    Enabled = False
    OnTimer = tmAutoSaverTimer
    Left = 256
    Top = 32
  end
  object dlgOpenMacro: TOpenDialog
    Filter = 'Cassava Macro (*.cms)|*.cms|'#12377#12409#12390#12398#12501#12449#12452#12523' (*.*)|*.*'
    Left = 480
    Top = 32
  end
  object PopMenuOpen: TPopupMenu
    OnPopup = PopMenuOpenPopup
    Left = 352
    Top = 32
  end
  object ActionList: TActionList
    Left = 224
    Top = 32
    object acSave: TAction
      Caption = #19978#26360#12365#20445#23384'(&S)'
      Enabled = False
      ImageIndex = 8
      OnExecute = acSaveExecute
    end
    object acUndo: TAction
      Caption = #20803#12395#25147#12377'(&U)'
      ImageIndex = 3
      OnExecute = acUndoExecute
      OnUpdate = acUndoUpdate
    end
    object acRedo: TAction
      Caption = #12420#12426#30452#12375'(&Y)'
      ImageIndex = 4
      OnExecute = acRedoExecute
      OnUpdate = acRedoUpdate
    end
    object acCut: TAction
      Caption = #20999#12426#21462#12426'(&T)'
      ImageIndex = 0
      OnExecute = acCutExecute
      OnUpdate = acCutUpdate
    end
    object acCopy: TAction
      Caption = #12467#12500#12540'(&C)'
      ImageIndex = 1
      OnExecute = acCopyExecute
      OnUpdate = acCopyUpdate
    end
    object acPaste: TAction
      Caption = #36028#12426#20184#12369'(&P)'
      ImageIndex = 2
      OnExecute = acPasteExecute
      OnUpdate = acPasteUpdate
    end
    object acMacroTerminate: TAction
      Caption = #23455#34892#12434#20013#26029'(&T)'
      OnExecute = acMacroTerminateExecute
      OnUpdate = acMacroTerminateUpdate
    end
  end
  object imcNormal: TImageCollection
    Images = <
      item
        Name = 'Item1'
        SourceImages = <
          item
            Image.Data = {
              89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
              61000000674944415478DAD591E10AC0200884F5C9B79EDC2921B8D1C5AD7E8C
              099161F7D9999A98EC8422807AC96B8ACE3F01C41EA29AD380DA1575A700992F
              0166DEC917342F9E12CBE458B1D02C841D9456EEA021E02948508542C0E8F2F7
              806AEBD50CD0E4E95F60E2026F537AF152CE794F0000000049454E44AE426082}
          end>
      end
      item
        Name = 'Item2'
        SourceImages = <
          item
            Image.Data = {
              89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
              61000000784944415478DAAD935112802008057927574F4E608D6511D0D8FBC1
              415D3727C1C4B41228005A8CC81CD200C93C01A4202E407BBD3A9009304E1DE3
              6A6E622A080D768B26B0F2E85D21A6C1996A02B4D7AB405206F3A7B5F80E3C83
              FB019F0CA68B3C6CDC1F296DF0BA801AA70C22C0920105C12FAF71251B84137E
              78751E98B10000000049454E44AE426082}
          end>
      end
      item
        Name = 'Item3'
        SourceImages = <
          item
            Image.Data = {
              89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
              61000000AB4944415478DAA593ED118020088661934663936C1347731313128F
              4AD42EFEE8F1F1FA88881932FC31EC0920BB8D951C1C0A3C0BA4A87AB0536A05
              79934300D8B6082911F09E8BB550F7315E71CE23A22622021C64B381DEB55444
              0EB1024AC00223EB1DD408146F657509EE4961C012E60444A934707FF7028FFA
              12FB9CC01360BFAC45644A8066107279532570E76046F0A4F944709FD48BC61D
              65E9F40A81F71B118EA56F8A7FBFF3095D2CB3765A4DC03B0000000049454E44
              AE426082}
          end>
      end
      item
        Name = 'Item4'
        SourceImages = <
          item
            Image.Data = {
              89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
              610000004E4944415478DA63FCCFF09F8112C038020C6064684451F09FA19E11
              AB012085C892308D181AD0D5211B00D380AE089BAB60F2180660B39564030819
              429401B80CC11906C4028A0DC03070D400060006A749F164BB4D690000000049
              454E44AE426082}
          end>
      end
      item
        Name = 'Item5'
        SourceImages = <
          item
            Image.Data = {
              89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
              610000004E4944415478DA63FCCFF09F8112C0380C0D6064684411F8CF50CF48
              B40120CDE81A6006228B23AB831B804D33BA4120797403893600DD7B64B9005B
              D8100C036C9A493680E85820078C1AC0C0000096A749F1A1C3B36C0000000049
              454E44AE426082}
          end>
      end
      item
        Name = 'Item6'
        SourceImages = <
          item
            Image.Data = {
              89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
              61000000194944415478DA63FCCFF09F8112C0386AC0A801A3060C170300B5C4
              1FF111842FAD0000000049454E44AE426082}
          end>
      end
      item
        Name = 'Item7'
        SourceImages = <
          item
            Image.Data = {
              89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
              61000000464944415478DA63FCCFF09F8112C008338011C4C40380EA18091A00
              04D86D6164C46908D10680E4C0349A2124B9009B77883200C335A3060C6B0308
              EA8602AC06900B28360000F85E6AF1B6DC41EF0000000049454E44AE426082}
          end>
      end
      item
        Name = 'Item8'
        SourceImages = <
          item
            Image.Data = {
              89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
              61000000734944415478DABD93510A002108057D37EFE66E864A41ABE5C2FA53
              9133BC8AC0C4F4A5702280B469F57E5C09049E215BFB58398225724114313B8E
              3433AB6F6C07F84EAE02B6E9EB886E0E0414C296EC3801C25B481208DC5A0E4A
              CF748935784950815D601BB7F02238A9ED2BFCF21BA37A0074EA5FF2B1FA52A3
              0000000049454E44AE426082}
          end>
      end
      item
        Name = 'Item9'
        SourceImages = <
          item
            Image.Data = {
              89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
              61000000564944415478DADD90E10A00101083EFDEDC9B636A9210933F565AB1
              FB5CF368D16EE400384C509EF50A08C10C0797CB1F73B60CD215C0F5066DF637
              00449FA9CD0D01BB923760E618C06EE40EDE02F870566307509500589B91F1E8
              CD1E2E0000000049454E44AE426082}
          end>
      end>
    Left = 8
    Top = 32
  end
  object imlNormal: TVirtualImageList
    DisabledGrayscale = True
    DisabledSuffix = '_Disabled'
    Images = <
      item
        CollectionIndex = 0
        CollectionName = 'Item1'
        Disabled = False
        Name = 'Item1'
      end
      item
        CollectionIndex = 1
        CollectionName = 'Item2'
        Disabled = False
        Name = 'Item2'
      end
      item
        CollectionIndex = 2
        CollectionName = 'Item3'
        Disabled = False
        Name = 'Item3'
      end
      item
        CollectionIndex = 3
        CollectionName = 'Item4'
        Disabled = False
        Name = 'Item4'
      end
      item
        CollectionIndex = 4
        CollectionName = 'Item5'
        Disabled = False
        Name = 'Item5'
      end
      item
        CollectionIndex = 5
        CollectionName = 'Item6'
        Disabled = False
        Name = 'Item6'
      end
      item
        CollectionIndex = 6
        CollectionName = 'Item7'
        Disabled = False
        Name = 'Item7'
      end
      item
        CollectionIndex = 7
        CollectionName = 'Item8'
        Disabled = False
        Name = 'Item8'
      end
      item
        CollectionIndex = 8
        CollectionName = 'Item9'
        Disabled = False
        Name = 'Item9'
      end>
    ImageCollection = imcNormal
    Left = 40
    Top = 32
  end
  object imcAdditional: TImageCollection
    Images = <
      item
        Name = 'Item1'
        SourceImages = <
          item
            Image.Data = {
              89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
              61000000924944415478DAAD92510E80300843E1E46327C711833252D044F971
              26EB6B81B192D297E20C601255122E054BB2340C013244E7241A639D2786B400
              73F77395A204B8BB090D54A52801D1BD4B0101D13DB683524040EEBD9B453BC4
              37855BB00F2809171F01F1B2035F0376572C6690D240FB4B6C9CCF4DCDEB7FAC
              15D986EE87D488510A6F631B629E4386790A77FF650B07C4F096F12F686E1B00
              00000049454E44AE426082}
          end>
      end
      item
        Name = 'Item2'
        SourceImages = <
          item
            Image.Data = {
              89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
              61000000694944415478DAAD92510EC0200843E9FD0FCD64230A263320F443E5
              83672D8289A9220800B25D68F4C200840122E7E85463073439605E2CE0BF7ECF
              1F60ACCACB03685E4D48DB9F53B0297190B332DB0011880FBCD341B4D943CC14
              F2D22994FF411BE0E6090AA8E8011C096BF169C297440000000049454E44AE42
              6082}
          end>
      end
      item
        Name = 'Item3'
        SourceImages = <
          item
            Image.Data = {
              89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
              610000006A4944415478DAED93410EC0200804D9FF3F7A6B941A9135D19E7A90
              A30E1320001A2D07D4A3073191348804A94D0C3108E8083B1E2505EEDCFB1F04
              A2D85A6E4D424B899CAC806C0C00178C2D80F3FFAF04AB1944C19DC1A719C445
              3A6C21AFF2760B47C724D0ED737E000127B2F12F5542590000000049454E44AE
              426082}
          end>
      end
      item
        Name = 'Item4'
        SourceImages = <
          item
            Image.Data = {
              89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
              610000006C4944415478DAAD92510EC0200843DB93032767C37DA8CB6682D81F
              21C49762A5C3511103C0383674DFE500080681C9D1AAE71B70C8817B6791FF7D
              ABDF80A8B00F18B74BAE3085D2C78D636AAE2A503588489B98F57A0178200540
              C241F90D0EA550FE07797DC49857197001DC9B8FEBADF4458A0000000049454E
              44AE426082}
          end>
      end
      item
        Name = 'Item5'
        SourceImages = <
          item
            Image.Data = {
              89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
              61000000784944415478DACD91D11680200843B97FEE9F53989921659E7A8817
              3D6E8CE15051795398007604B56254A2E3EC5823A09926D51181408F850EEE96
              62E440B5B443DF5D30602CB091CE93755EC04819C33803818F5268A7A4946A83
              DDA75710F70BD302930EA2D48E675CACCF5270023F5EE1B25A1021A22CC028A8
              F106A5F1050000000049454E44AE426082}
          end>
      end
      item
        Name = 'Item6'
        SourceImages = <
          item
            Image.Data = {
              89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
              610000008C4944415478DAA5924B0EC02008057D27174F6EC588A160FD959589
              CC002272C8E14F6025A04839A554CF31C64089B02D40B9664807CB0A83A540C3
              BAAA742492A18093FA99A81BED3D8B8702A95E2B4B8212480E77B116D860A024
              DC091ABC35825B9B82A78F585B2BCD0170BBB7B01358D8C6F4238D605DE92BD0
              46BB825F02F3D85BB01BE1141E6EE1341EB71C8CF1B0DDF5C20000000049454E
              44AE426082}
          end>
      end
      item
        Name = 'Item7'
        SourceImages = <
          item
            Image.Data = {
              89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
              61000000834944415478DAAD93010E80200845E1E4C0C9C94AA7E4775AC6E6DC
              101E20C04E4E3BC263008307E705002FA45440EC0F40EBECACA26E662422A4A6
              8CE00D203A5F9A46E721FDAA07806A8801318B0CE89D316058020658D26BBACF
              23A0035340FF892F01A50C87D16F9F29602C0180BB302EA1DA7E988368B73189
              39C8DE2EFCB08D0780B379F18BC333DE0000000049454E44AE426082}
          end>
      end
      item
        Name = 'Item8'
        SourceImages = <
          item
            Image.Data = {
              89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
              61000000884944415478DAA5D3D10E8020080550EF972B5F4E6DE824BD885B3E
              95C8016C418B963F0B310012505C00B8686940D005F0C97B35863B204BE61001
              EE931D3093F13E6B8884237C815A6B69D228B2C653207D3F01AC9BB927E711A4
              1FF06BEC13605E10FA415ED57725563003E23BD800435807A7F80768B5E93AE3
              FE190FC0402CC8FFC619B7F500781D9BF1551EA9B30000000049454E44AE4260
              82}
          end>
      end
      item
        Name = 'Item9'
        SourceImages = <
          item
            Image.Data = {
              89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
              610000004F4944415478DA63FCCFF09F8112C038380D6064206CEA7FB0323C2E
              8019025388538C140360E2040D40D78CAE89E830A0D8006C8146D000526CA78F
              0184008A01A46A4631003DE0C8720139806203000ECF41F154A4448300000000
              49454E44AE426082}
          end>
      end
      item
        Name = 'Item10'
        SourceImages = <
          item
            Image.Data = {
              89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
              61000000314944415478DA63646060F8CF4001600419F01F99832E49804F1D03
              46BD40C005FF11228C8C8C187CEA1830EA85A1EE05001E065001B2EA2F270000
              000049454E44AE426082}
          end>
      end
      item
        Name = 'Item11'
        SourceImages = <
          item
            Image.Data = {
              89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
              61000000384944415478DAEDCF310A00300CC340E9FF8F76E9D46E8112E8126F
              5E0E640862789C07D886DC9250FE4EA02B21C7D2FA77029330091F1316C7A988
              01FAA1F2410000000049454E44AE426082}
          end>
      end
      item
        Name = 'Item12'
        SourceImages = <
          item
            Image.Data = {
              89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
              61000000354944415478DA63FCCFF09F8112C008C414990035E03F2A17A7F998
              7CEAB800C5FEFF08F318191909F2A963C0A81746BD30E05E00009F3C6500EE00
              841E0000000049454E44AE426082}
          end>
      end
      item
        Name = 'Item13'
        SourceImages = <
          item
            Image.Data = {
              89504E470D0A1A0A0000000D49484452000000100000001008060000001FF3FF
              61000000764944415478DAB5938D0AC0200884F5C9DB9EDC15B47173A7183121
              B21FBFF42A3531D931AD00CEB16D5A13D325C01D3C02D12F0158809F5B067C34
              E8ED215872D27F005F022A8E16814A1968F7710DC71490057B08BD050608FB4C
              039F01F34340F31BA3FE981A844FB5A241F65918E455CAEE77BE00BD348DF157
              AFC5090000000049454E44AE426082}
          end>
      end>
    Left = 104
    Top = 32
  end
  object imlAdditional: TVirtualImageList
    AutoFill = True
    DisabledGrayscale = False
    DisabledSuffix = '_Disabled'
    Images = <
      item
        CollectionIndex = 0
        CollectionName = 'Item1'
        Disabled = False
        Name = 'Item1'
      end
      item
        CollectionIndex = 1
        CollectionName = 'Item2'
        Disabled = False
        Name = 'Item2'
      end
      item
        CollectionIndex = 2
        CollectionName = 'Item3'
        Disabled = False
        Name = 'Item3'
      end
      item
        CollectionIndex = 3
        CollectionName = 'Item4'
        Disabled = False
        Name = 'Item4'
      end
      item
        CollectionIndex = 4
        CollectionName = 'Item5'
        Disabled = False
        Name = 'Item5'
      end
      item
        CollectionIndex = 5
        CollectionName = 'Item6'
        Disabled = False
        Name = 'Item6'
      end
      item
        CollectionIndex = 6
        CollectionName = 'Item7'
        Disabled = False
        Name = 'Item7'
      end
      item
        CollectionIndex = 7
        CollectionName = 'Item8'
        Disabled = False
        Name = 'Item8'
      end
      item
        CollectionIndex = 8
        CollectionName = 'Item9'
        Disabled = False
        Name = 'Item9'
      end
      item
        CollectionIndex = 9
        CollectionName = 'Item10'
        Disabled = False
        Name = 'Item10'
      end
      item
        CollectionIndex = 10
        CollectionName = 'Item11'
        Disabled = False
        Name = 'Item11'
      end
      item
        CollectionIndex = 11
        CollectionName = 'Item12'
        Disabled = False
        Name = 'Item12'
      end
      item
        CollectionIndex = 12
        CollectionName = 'Item13'
        Disabled = False
        Name = 'Item13'
      end>
    ImageCollection = imcAdditional
    Left = 136
    Top = 32
  end
  object imlNormalDisabled: TVirtualImageList
    DisabledOpacity = 32
    DisabledGrayscale = True
    DisabledSuffix = '_Disabled'
    Images = <
      item
        CollectionIndex = 0
        CollectionName = 'Item1'
        Disabled = True
        Name = 'Item1_Disabled'
      end
      item
        CollectionIndex = 1
        CollectionName = 'Item2'
        Disabled = True
        Name = 'Item2_Disabled'
      end
      item
        CollectionIndex = 2
        CollectionName = 'Item3'
        Disabled = True
        Name = 'Item3_Disabled'
      end
      item
        CollectionIndex = 3
        CollectionName = 'Item4'
        Disabled = True
        Name = 'Item4_Disabled'
      end
      item
        CollectionIndex = 4
        CollectionName = 'Item5'
        Disabled = True
        Name = 'Item5_Disabled'
      end
      item
        CollectionIndex = 5
        CollectionName = 'Item6'
        Disabled = True
        Name = 'Item6_Disabled'
      end
      item
        CollectionIndex = 6
        CollectionName = 'Item7'
        Disabled = True
        Name = 'Item7_Disabled'
      end
      item
        CollectionIndex = 7
        CollectionName = 'Item8'
        Disabled = True
        Name = 'Item8_Disabled'
      end
      item
        CollectionIndex = 8
        CollectionName = 'Item9'
        Disabled = True
        Name = 'Item9_Disabled'
      end>
    ImageCollection = imcNormal
    Left = 72
    Top = 32
  end
end
