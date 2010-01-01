/* pseudo-reloc.c

   Contributed by Egor Duda  <deo@logos-m.ru>
   Modified by addition of runtime_pseudo_reloc version 2
   by Kai Tietz  <kai.tietz@onevision.com>
	
   THIS SOFTWARE IS NOT COPYRIGHTED

   This source code is offered for use in the public domain. You may
   use, modify or distribute it freely.

   This code is distributed in the hope that it will be useful but
   WITHOUT ANY WARRANTY. ALL WARRENTIES, EXPRESS OR IMPLIED ARE HEREBY
   DISCLAMED. This includes but is not limited to warrenties of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
 
 extern char __RUNTIME_PSEUDO_RELOC_LIST__;
 extern char __RUNTIME_PSEUDO_RELOC_LIST_END__;
 extern char _image_base__;
 extern char __text_start__;
 
typedef struct {
  DWORD addend;
  DWORD target;
} runtime_pseudo_reloc_item_v1;

typedef struct {
  DWORD sym;
  DWORD target;
  DWORD flags;
} runtime_pseudo_reloc_item_v2;

typedef struct {
  DWORD magic1;
  DWORD magic2;
  DWORD version;
} runtime_pseudo_reloc_v2;

static void
__write_memory (void *addr,const void *src,size_t len)
{
  MEMORY_BASIC_INFORMATION b;
  DWORD oldprot;
  if (!len)
    return;
  assert (VirtualQuery (addr, &b, sizeof(b)));
  /* Temporarily allow write access to read-only protected memory.  */
  if (b.Protect != PAGE_EXECUTE_READWRITE && b.Protect != PAGE_READWRITE)
    VirtualProtect (b.BaseAddress, b.RegionSize, PAGE_EXECUTE_READWRITE,
		  &oldprot);
  memcpy (addr, src, len);
  if (b.Protect != PAGE_EXECUTE_READWRITE && b.Protect != PAGE_READWRITE)
    VirtualProtect (b.BaseAddress, b.RegionSize, oldprot, &oldprot);
}

#define RP_VERSION_V1 0
#define RP_VERSION_V2 1
 
static void
do_pseudo_reloc (void * start, void * end, void * base)
{
  ptrdiff_t addr_imp, reldata;
  ptrdiff_t reloc_target = (ptrdiff_t) ((char *)end - (char*)start);
  runtime_pseudo_reloc_v2 *v2_hdr = (runtime_pseudo_reloc_v2 *) start;
  runtime_pseudo_reloc_item_v2 *r;

  if (reloc_target < 8)
    return;
  /* Check if this is old version pseudo relocation version.  */
  if (reloc_target >= 12
      && v2_hdr->magic1 == 0 && v2_hdr->magic2 == 0
      && v2_hdr->version == RP_VERSION_V1)
    v2_hdr++;
  if (v2_hdr->magic1 != 0 || v2_hdr->magic2 != 0)
    {
      runtime_pseudo_reloc_item_v1 * o;
      for (o = (runtime_pseudo_reloc_item_v1 *) v2_hdr;
	   o < (runtime_pseudo_reloc_item_v1 *)end;
           o++)
	{
	  DWORD newval;
	  reloc_target = (ptrdiff_t) base + o->target;
	  newval = (*((DWORD*) reloc_target)) + o->addend;
	  __write_memory ((void *) reloc_target, &newval, sizeof(DWORD));
	}
      return;
    }

  /* Check if this is a known version.  */
  if (v2_hdr->version != RP_VERSION_V2)
    {
#ifdef DEBUG
      fprintf (stderr, "internal mingw runtime error:"
	       "psuedo_reloc version %d is unknown to this runtime.\n",
	       (int) v2_hdr->version);
#endif
      return;
    }

  /* Walk over header.  */
  r = (runtime_pseudo_reloc_item_v2 *) &v2_hdr[1];

  for (; r < (runtime_pseudo_reloc_item_v2 *) end; r++)
    {
      reloc_target = (ptrdiff_t) base + r->target;
      addr_imp = (ptrdiff_t) base + r->sym;
      addr_imp = *((ptrdiff_t *) addr_imp);

      switch ((r->flags & 0xff))
        {
          case 8:
	    reldata = (ptrdiff_t) (*((unsigned char *)reloc_target));
	    if ((reldata & 0x80) != 0)
	      reldata |= ~((ptrdiff_t) 0xff);
	    break;
	  case 16:
	    reldata = (ptrdiff_t) (*((unsigned short *)reloc_target));
	    if ((reldata & 0x8000) != 0)
	      reldata |= ~((ptrdiff_t) 0xffff);
	    break;
	  case 32:
	    reldata = (ptrdiff_t) (*((unsigned int *)reloc_target));
#ifdef _WIN64
	    if ((reldata & 0x80000000) != 0)
	      reldata |= ~((ptrdiff_t) 0xffffffff);
#endif
	    break;
#ifdef _WIN64
	  case 64:
	    reldata = (ptrdiff_t) (*((unsigned long long *)reloc_target));
	    break;
#endif
	  default:
	    reldata=0;
#ifdef DEBUG
    	    fprintf(stderr, "internal mingw runtime error: "
		    "unknown pseudo_reloc bit size %d\n",
		    (int) (r->flags & 0xff));
#endif
	    break;
        }
      reldata -= ((ptrdiff_t) base + r->sym);
      reldata += addr_imp;
      switch ((r->flags & 0xff))
        {
         case 8:
           __write_memory ((void *) reloc_target, &reldata, 1);
	   break;
	 case 16:
           __write_memory ((void *) reloc_target, &reldata, 2);
	   break;
	 case 32:
           __write_memory ((void *) reloc_target, &reldata, 4);
	   break;
#ifdef _WIN64
	 case 64:
           __write_memory ((void *) reloc_target, &reldata, 8);
	   break;
#endif
        }
     }
 }
 
void
 _pei386_runtime_relocator ()
{
  static int was_init = 0;
#ifdef UNDER_CE
  static volatile void *text_start;
  static volatile int pe_header_size;
#endif
  void *image_base;
  if (was_init)
    return;
  ++was_init;

#ifndef UNDER_CE
  image_base = _image_base__;
#else
  /* The loader of Windows Mobile 6.1 and above refuses to apply base
     relocations if the resulting RVA falls out of the image.
     __image_base__ happens to be one such symbol, as most other ld
     magic symbols (__dll__, __major_image_version__, etc.).
     Basically, symbols that end up on the absolute section.  As an
     alternative to get at the image base, we relocate against the
     __text_start__ symbol, and subtract the PE header from that.  In
     practice, this gives us the image base.  We go through volatile
     globals to make sure gcc doesn't fold the 0x1000 subtraction into
     the relocatable address, as that would be the same as relocating
     against __image_base__ in the first place, exactly what we need
     to avoid.  */
  text_start = &__text_start__;
  pe_header_size = 0x1000;
  image_base = (char *) text_start - pe_header_size;
#endif

  do_pseudo_reloc (&__RUNTIME_PSEUDO_RELOC_LIST__,
		   &__RUNTIME_PSEUDO_RELOC_LIST_END__,
		   image_base);
}
