#pragma once

// Private DWM API datastructures and routines
typedef struct _D3DKMT_OPENADAPTERFROMDEVICENAME
{
  wchar_t	*pDeviceName;
  HANDLE	hAdapter;
  LUID		AdapterLuid;
} D3DKMT_OPENADAPTERFROMDEVICENAME;

typedef struct _D3DKMT_CLOSEADAPTER
{
  HANDLE	hAdapter;
} D3DKMT_CLOSEADAPTER;

typedef struct _D3DKMT_QUERYADAPTERINFO
{
  HANDLE			hAdapter;
  unsigned int	Type;
  void			*pPrivateDriverData;
  unsigned int	PrivateDriverDataSize;
} D3DKMT_QUERYADAPTERINFO;

typedef struct _D3DKMT_OPENADAPTERFROMGDIDISPLAYNAME
{
  WCHAR	DeviceName[32];
  HANDLE	hAdapter;
  LUID	AdapterLuid;
  unsigned int	VidPnSourceId;
} D3DKMT_OPENADAPTERFROMGDIDISPLAYNAME;

typedef struct _D3DKMT_SEGMENTSIZEINFO
{
  ULONGLONG	DedicatedVideoMemorySize;
  ULONGLONG	DedicatedSystemMemorySize;
  ULONGLONG	SharedSystemMemorySize;
} D3DKMT_SEGMENTSIZEINFO;

typedef struct _D3DKMT_ADAPTERREGISTRYINFO
{
  WCHAR	AdapterString[MAX_PATH];
  WCHAR	BiosString[MAX_PATH];
  WCHAR	DacType[MAX_PATH];
  WCHAR	ChipType[MAX_PATH];
} D3DKMT_ADAPTERREGISTRYINFO;