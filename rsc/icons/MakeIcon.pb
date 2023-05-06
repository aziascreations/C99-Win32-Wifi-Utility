;- Remarks
; The code is rough at best, but it works.
; That's good enough for me.
; The output icon only has PNG entries.
; This can cause Windows to not display it properly in the photo app and explorer.
; However, when "applied" to an app, it works just fine.
; The sizes 48 and 96 aren't included since Windows can get a scaled version easily from 128 and 64.
; It also makes the icon ~13kb instead of ~18kb.

;- Compiler Directives
EnableExplicit

;- Constants
#IconCli16 = "./pngs/wifi-cli_16_lossy.png"
#IconCli32 = "./pngs/wifi-cli_32_lossy.png"
;#IconCli48 = "./pngs/wifi-cli_48_lossy.png"
#IconCli64 = "./pngs/wifi-cli_64_lossy.png"
;#IconCli96 = "./pngs/wifi-cli_96_lossy.png"
#IconCli128 = "./pngs/wifi-cli_128_lossy.png"
#IconCli256 = "./pngs/wifi-cli_256_lossy.png"

#OutputCli$ = "wifi-cli.ico"

#IconCount = 5

;- Structures
Structure ICONDIR
	Reserved.w
	Type.w
	Count.w
EndStructure

Structure ICONDIRENTRY
	Width.a ; 0 for 256
	Height.a; 0 for 256
	ColorCount.a ; Set to 0 for png, I guess...
	Reserved.a	 ; Leave as 0
	ColorPlanes.w
	BitsPerPixel.w
	DataSize.l
	DataOffset.l
EndStructure

;- Procedures
Procedure.i LoadIconToBuffer(Filepath$)
	Define *Buffer = AllocateMemory(FileSize(Filepath$))
	Debug "Loading "+MemorySize(*Buffer)+" bytes from '"+Filepath$+"' into "+Str(*Buffer)
	
	OpenFile(0, Filepath$)
	ReadData(0, *Buffer, MemorySize(*Buffer))
	CloseFile(0)
	
	ProcedureReturn *Buffer
EndProcedure

Procedure PrepareDirEntry(*DirEntry.ICONDIRENTRY, *IconData, Offset.i, Size.a)
	Debug "Preparing entry for size '"+Str(Size)+"' and data from "+Str(*IconData)
	With *DirEntry
		\Width = Size
		\Height = Size
		\ColorCount = 0
		\Reserved = 0
		\ColorPlanes = 1
		\BitsPerPixel = 32
		\DataSize = MemorySize(*IconData)
		\DataOffset = Offset
	EndWith
EndProcedure

;- Code

;-> Clearing
DeleteFile(#OutputCli$, #PB_FileSystem_Force)

;-> Preparing variables & structures
Define TmpIndex.i

Define IconHeader.ICONDIR
IconHeader\Reserved = 0
IconHeader\Type = 1
IconHeader\Count = #IconCount

NewList IconsData.i()
AddElement(IconsData()) : IconsData() = LoadIconToBuffer(#IconCli16)
AddElement(IconsData()) : IconsData() = LoadIconToBuffer(#IconCli32)
; AddElement(IconsData()) : IconsData() = LoadIconToBuffer(#IconCli48)
AddElement(IconsData()) : IconsData() = LoadIconToBuffer(#IconCli64)
; AddElement(IconsData()) : IconsData() = LoadIconToBuffer(#IconCli96)
AddElement(IconsData()) : IconsData() = LoadIconToBuffer(#IconCli128)
AddElement(IconsData()) : IconsData() = LoadIconToBuffer(#IconCli256)

Define CurrentOffset.i = SizeOf(ICONDIR) + (SizeOf(ICONDIRENTRY) * #IconCount)
Define *IconDataTmp
NewList IconDirEntries.ICONDIRENTRY()

TmpIndex = 0

PrepareDirEntry(AddElement(IconDirEntries()), PeekI(SelectElement(IconsData(), TmpIndex)), CurrentOffset, 16)
CurrentOffset = CurrentOffset + MemorySize(PeekI(SelectElement(IconsData(), TmpIndex)))
TmpIndex = TmpIndex + 1

PrepareDirEntry(AddElement(IconDirEntries()), PeekI(SelectElement(IconsData(), TmpIndex)), CurrentOffset, 32)
CurrentOffset = CurrentOffset + MemorySize(PeekI(SelectElement(IconsData(), TmpIndex)))
TmpIndex = TmpIndex + 1

; PrepareDirEntry(AddElement(IconDirEntries()), PeekI(SelectElement(IconsData(), TmpIndex)), CurrentOffset, 48)
; CurrentOffset = CurrentOffset + MemorySize(PeekI(SelectElement(IconsData(), TmpIndex)))
; TmpIndex = TmpIndex + 1

PrepareDirEntry(AddElement(IconDirEntries()), PeekI(SelectElement(IconsData(), TmpIndex)), CurrentOffset, 64)
CurrentOffset = CurrentOffset + MemorySize(PeekI(SelectElement(IconsData(), TmpIndex)))
TmpIndex = TmpIndex + 1

; PrepareDirEntry(AddElement(IconDirEntries()), PeekI(SelectElement(IconsData(), TmpIndex)), CurrentOffset, 96)
; CurrentOffset = CurrentOffset + MemorySize(PeekI(SelectElement(IconsData(), TmpIndex)))
; TmpIndex = TmpIndex + 1

PrepareDirEntry(AddElement(IconDirEntries()), PeekI(SelectElement(IconsData(), TmpIndex)), CurrentOffset, 128)
CurrentOffset = CurrentOffset + MemorySize(PeekI(SelectElement(IconsData(), TmpIndex)))
TmpIndex = TmpIndex + 1

PrepareDirEntry(AddElement(IconDirEntries()), PeekI(SelectElement(IconsData(), TmpIndex)), CurrentOffset, 0)  ; 0 means 256
CurrentOffset = CurrentOffset + MemorySize(PeekI(SelectElement(IconsData(), TmpIndex)))
TmpIndex = TmpIndex + 1

;-> Writing data
Define i.i

CreateFile(0, #OutputCli$)

WriteData(0, @IconHeader, SizeOf(ICONDIR))

For i=0 To #IconCount - 1
	WriteData(0, SelectElement(IconDirEntries(), i), SizeOf(ICONDIRENTRY))
Next

For i=0 To #IconCount - 1
	Define *IconDataBuffer = PeekI(SelectElement(IconsData(), i))
	WriteData(0, *IconDataBuffer, MemorySize(*IconDataBuffer))
Next

CloseFile(0)

;-> Clearing memory
ForEach IconsData()
	FreeMemory(IconsData())
Next

;-> Exiting
Debug "Done !"
End 0
