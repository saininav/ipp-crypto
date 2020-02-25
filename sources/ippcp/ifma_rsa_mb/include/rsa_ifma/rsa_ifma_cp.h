/*******************************************************************************
* Copyright 2019-2020 Intel Corporation
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
//  Purpose: MB RSA. Public definitions and declarations
//
*/

#ifndef IFMA_RSA_CP_H
#define IFMA_RSA_CP_H

#include "rsa_ifma_defs.h"
#include "rsa_ifma_status.h"
#include "ifma_method.h"

EXTERN_C ifma_status ifma_cp_rsa52_public_mb8(const int8u* const from_pa[8],
                                                    int8u* const to_pa[8],
                                              const int64u* const n_pa[8],
                                                    int rsaBitlen,
                                              const ifma_RSA_Method* m,
                                                    int8u* pBuffer);

EXTERN_C ifma_status ifma_cp_rsa52_private_mb8(const int8u* const from_pa[8],
                                                     int8u* const to_pa[8],
                                               const int64u* const d_pa[8],
                                               const int64u* const n_pa[8],
                                                     int rsaBitlen,
                                               const ifma_RSA_Method* m,
                                                     int8u* pBuffer);

EXTERN_C ifma_status ifma_cp_rsa52_private_ctr_mb8(const int8u* const from_pa[8],
                                                         int8u* const to_pa[8],
                                                   const int64u* const p_pa[8],
                                                   const int64u* const q_pa[8],
                                                   const int64u* const dp_pa[8],
                                                   const int64u* const dq_pa[8],
                                                   const int64u* const iq_pa[8],
                                                         int rsaBitlen,
                                                   const ifma_RSA_Method* m,
                                                         int8u* pBuffer);

#endif /* IFMA_RSA_CP_H */
