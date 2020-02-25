/*******************************************************************************
* Copyright 2010-2020 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/* 
// 
//  Purpose:
//     Intel(R) Integrated Performance Primitives. Cryptography Primitives.
//     Internal EC over GF(p^m) basic Definitions & Function Prototypes
// 
//     Context:
//        p521r1_select_ap_w5()
//
*/

#include "owndefs.h"
#include "owncp.h"
#include "pcpgfpecstuff.h"
#include "pcpmask_ct.h"

/*
// select affine point
*/
#if (_IPP32E < _IPP32E_M7)
void p521r1_select_ap_w5(BNU_CHUNK_T* pVal, const BNU_CHUNK_T* pTbl, int idx)
{
   #define OPERAND_BITSIZE (521)
   #define LEN_P521        (BITS_BNU_CHUNK(OPERAND_BITSIZE))
   #define LEN_P521_APOINT (2*LEN_P521)

   const int tblLen = 16;
   int i;
   unsigned int n;

   /* clear output affine point */
   for(n=0; n<LEN_P521_APOINT; n++)
      pVal[n] = 0;

   /* select point */
   for(i=1; i<=tblLen; i++) {
      BNU_CHUNK_T mask = cpIsEqu_ct(i, idx);
      for(n=0; n<LEN_P521_APOINT; n++)
         pVal[n] |= (pTbl[n] & mask);
      pTbl += LEN_P521_APOINT;
   }

   #undef OPERAND_BITSIZE
   #undef LEN_P521
   #undef P521_POINT_AFFINE
}
#endif
