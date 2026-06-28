/*
 * Copyright 2025 NIBE AB
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
 */
#ifndef SRC_SHIP_SHIP_CONNECTION_SERVER_H_
#define SRC_SHIP_SHIP_CONNECTION_SERVER_H_

#include "src/ship/ship_connection/ship_connection_internal.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief State machine for handling server states
 * @param self SHIP Connection insatnce to handle the state with
 */
void ServerHandleState(ShipConnection* self);

/**
 * @brief CMI_SERVER_WAIT state handling
 * @param self SHIP Connection insatnce to handle the state with
 */
void CmiStateServerWait(ShipConnection* self);

/**
 * @brief CMI_STATE_SERVER_EVALUATE state handling
 * @param self SHIP Connection insatnce to handle the state with
 */
void CmiStateServerEvaluate(ShipConnection* self);

/**
 * @brief SME_PROT_H_STATE_SERVER_INIT state handling
 * @param self SHIP Connection insatnce to handle the state with
 */
void SmeProtHandshakeStateServerInit(ShipConnection* self);

/**
 * @brief SME_PROT_H_STATE_SERVER_LISTEN_PROPOSAL state handling
 * @param self SHIP Connection insatnce to handle the state with
 */
void SmeProtHandshakeStateServerListenProposal(ShipConnection* self);

/**
 * @brief SME_PROT_H_STATE_SERVER_LISTEN_CONFIRM state handling
 * @param self SHIP Connection insatnce to handle the state with
 */
void SmeProtHandshakeStateServerListenConfirm(ShipConnection* self);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_SHIP_CONNECTION_SERVER_H_
