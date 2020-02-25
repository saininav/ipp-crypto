/*******************************************************************************
* Copyright 2013-2020 Intel Corporation
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
//     Cryptography Primitive.
//     AES-GCM
// 
//  Contents:
//        ippsAES_GCMProcessAAD()
//
*/

#include "owndefs.h"
#include "owncp.h"
#include "pcpaesauthgcm.h"
#include "pcpaesm.h"
#include "pcptool.h"

#if (_ALG_AES_SAFE_==_ALG_AES_SAFE_COMPACT_SBOX_)
#  include "pcprijtables.h"
#endif

/*F*
//    Name: ippsAES_GCMProcessAAD
//
// Purpose: AAD processing.
//
// Returns:                Reason:
//    ippStsNullPtrErr        pState == NULL
//                            pAAD == NULL, aadLen>0
//    ippStsContextMatchErr   !AESGCM_VALID_ID()
//    ippStsLengthErr         aadLen <0
//    ippStsBadArgErr         illegal sequence call
//    ippStsNoErr             no errors
//
// Parameters:
//    pAAD        pointer to the AAD
//    aadlen      length of AAD (it could be 0)
//    pState      pointer to the context
//
*F*/
IPPFUN(IppStatus, ippsAES_GCMProcessAAD,(const Ipp8u* pAAD, int aadLen, IppsAES_GCMState* pState))
{
   /* test pState pointer */
   IPP_BAD_PTR1_RET(pState);
   /* use aligned context */
   pState = (IppsAES_GCMState*)( IPP_ALIGNED_PTR(pState, AESGCM_ALIGNMENT) );
   /* test if context is valid */
   IPP_BADARG_RET(!AESGCM_VALID_ID(pState), ippStsContextMatchErr);

   /* test AAD pointer and length */
   IPP_BADARG_RET(aadLen && !pAAD, ippStsNullPtrErr);
   IPP_BADARG_RET(aadLen<0, ippStsLengthErr);

   IPP_BADARG_RET(!(GcmIVprocessing==AESGCM_STATE(pState) || GcmAADprocessing==AESGCM_STATE(pState)), ippStsBadArgErr);

   {
      /* get method */
      MulGcm_ hashFunc = AESGCM_HASH(pState);

      if( GcmIVprocessing==AESGCM_STATE(pState) ) {
         IPP_BADARG_RET(0==AESGCM_IV_LEN(pState), ippStsBadArgErr);

         /* complete IV processing */
         if(CTR_POS==AESGCM_IV_LEN(pState)) {
            /* apply special format if IV length is 12 bytes */
            AESGCM_COUNTER(pState)[12] = 0;
            AESGCM_COUNTER(pState)[13] = 0;
            AESGCM_COUNTER(pState)[14] = 0;
            AESGCM_COUNTER(pState)[15] = 1;
         }
         else {
            /* process the rest of IV */
            if(AESGCM_BUFLEN(pState))
               hashFunc(AESGCM_COUNTER(pState), AESGCM_HKEY(pState), AesGcmConst_table);

            /* add IV bit length */
            {
               Ipp64u ivBitLen = AESGCM_IV_LEN(pState)*BYTESIZE;
               Ipp8u tmp[BLOCK_SIZE];
               PadBlock(0, tmp, BLOCK_SIZE-8);
               U32_TO_HSTRING(tmp+8,  IPP_HIDWORD(ivBitLen));
               U32_TO_HSTRING(tmp+12, IPP_LODWORD(ivBitLen));
               XorBlock16(tmp, AESGCM_COUNTER(pState), AESGCM_COUNTER(pState));
               hashFunc(AESGCM_COUNTER(pState), AESGCM_HKEY(pState), AesGcmConst_table);
            }
         }

         /* prepare initial counter */
         {
            IppsAESSpec* pAES = AESGCM_CIPHER(pState);
            RijnCipher encoder = RIJ_ENCODER(pAES);
            //encoder((Ipp32u*)AESGCM_COUNTER(pState), (Ipp32u*)AESGCM_ECOUNTER0(pState), RIJ_NR(pAES), RIJ_EKEYS(pAES), (const Ipp32u (*)[256])RIJ_ENC_SBOX(pAES));
            #if (_ALG_AES_SAFE_==_ALG_AES_SAFE_COMPACT_SBOX_)
            encoder(AESGCM_COUNTER(pState), AESGCM_ECOUNTER0(pState), RIJ_NR(pAES), RIJ_EKEYS(pAES), RijEncSbox/*NULL*/);
            #else
            encoder(AESGCM_COUNTER(pState), AESGCM_ECOUNTER0(pState), RIJ_NR(pAES), RIJ_EKEYS(pAES), NULL);
            #endif
         }

         /* switch mode and init counters */
         AESGCM_STATE(pState) = GcmAADprocessing;
         AESGCM_AAD_LEN(pState) = CONST_64(0);
         AESGCM_BUFLEN(pState) = 0;
      }

      /*
      // AAD processing
      */

      /* test if buffer is not empty */
      if(AESGCM_BUFLEN(pState)) {
         int locLen = IPP_MIN(aadLen, BLOCK_SIZE-AESGCM_BUFLEN(pState));
         XorBlock(pAAD, AESGCM_GHASH(pState)+AESGCM_BUFLEN(pState), AESGCM_GHASH(pState)+AESGCM_BUFLEN(pState), locLen);
         AESGCM_BUFLEN(pState) += locLen;

         /* if buffer full */
         if(BLOCK_SIZE==AESGCM_BUFLEN(pState)) {
            hashFunc(AESGCM_GHASH(pState), AESGCM_HKEY(pState), AesGcmConst_table);
            AESGCM_BUFLEN(pState) = 0;
         }

         AESGCM_AAD_LEN(pState) += (Ipp64u)locLen;
         pAAD += locLen;
         aadLen -= locLen;
      }

      /* process main part of AAD */
      {
         int lenBlks = aadLen & (-BLOCK_SIZE);
         if(lenBlks) {
            Auth_ authFunc = AESGCM_AUTH(pState);

            authFunc(AESGCM_GHASH(pState), pAAD, lenBlks, AESGCM_HKEY(pState), AesGcmConst_table);

            AESGCM_AAD_LEN(pState) += (Ipp64u)lenBlks;
            pAAD += lenBlks;
            aadLen -= lenBlks;
         }
      }

      /* copy the rest of AAD into the buffer */
      if(aadLen) {
         XorBlock(pAAD, AESGCM_GHASH(pState), AESGCM_GHASH(pState), aadLen);
         AESGCM_AAD_LEN(pState) += (Ipp64u)aadLen;
         AESGCM_BUFLEN(pState) = aadLen;
      }

      return ippStsNoErr;
   }
}
