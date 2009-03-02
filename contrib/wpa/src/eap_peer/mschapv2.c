/*
 * MSCHAPV2 (RFC 2759)
 * Copyright (c) 2004-2008, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#include "includes.h"

#include "common.h"
#include "ms_funcs.h"
#include "mschapv2.h"

const u8 * mschapv2_remove_domain(const u8 *username, size_t *len)
{
	size_t i;

	/*
	 * MSCHAPv2 does not include optional domain name in the
	 * challenge-response calculation, so remove domain prefix
	 * (if present).
	 */

	for (i = 0; i < *len; i++) {
		if (username[i] == '\\') {
			*len -= i + 1;
			return username + i + 1;
		}
	}

	return username;
}


void mschapv2_derive_response(const u8 *identity, size_t identity_len,
			      const u8 *password, size_t password_len,
			      int pwhash,
			      const u8 *auth_challenge,
			      const u8 *peer_challenge,
			      u8 *nt_response, u8 *auth_response,
			      u8 *master_key)
{
	const u8 *username;
	size_t username_len;
	u8 password_hash[16], password_hash_hash[16];

	wpa_hexdump_ascii(MSG_DEBUG, "MSCHAPV2: Identity",
			  identity, identity_len);
	username_len = identity_len;
	username = mschapv2_remove_domain(identity, &username_len);
	wpa_hexdump_ascii(MSG_DEBUG, "MSCHAPV2: Username",
			  username, username_len);

	wpa_hexdump(MSG_DEBUG, "MSCHAPV2: auth_challenge",
		    auth_challenge, MSCHAPV2_CHAL_LEN);
	wpa_hexdump(MSG_DEBUG, "MSCHAPV2: peer_challenge",
		    peer_challenge, MSCHAPV2_CHAL_LEN);
	wpa_hexdump_ascii(MSG_DEBUG, "MSCHAPV2: username",
			  username, username_len);
	/* Authenticator response is not really needed yet, but calculate it
	 * here so that challenges need not be saved. */
	if (pwhash) {
		wpa_hexdump_key(MSG_DEBUG, "MSCHAPV2: password hash",
				password, password_len);
		generate_nt_response_pwhash(auth_challenge, peer_challenge,
					    username, username_len,
					    password, nt_response);
		generate_authenticator_response_pwhash(
			password, peer_challenge, auth_challenge,
			username, username_len, nt_response, auth_response);
	} else {
		wpa_hexdump_ascii_key(MSG_DEBUG, "MSCHAPV2: password",
				      password, password_len);
		generate_nt_response(auth_challenge, peer_challenge,
				     username, username_len,
				     password, password_len, nt_response);
		generate_authenticator_response(password, password_len,
						peer_challenge, auth_challenge,
						username, username_len,
						nt_response, auth_response);
	}
	wpa_hexdump(MSG_DEBUG, "MSCHAPV2: NT Response",
		    nt_response, MSCHAPV2_NT_RESPONSE_LEN);
	wpa_hexdump(MSG_DEBUG, "MSCHAPV2: Auth Response",
		    auth_response, MSCHAPV2_AUTH_RESPONSE_LEN);

	/* Generate master_key here since we have the needed data available. */
	if (pwhash) {
		hash_nt_password_hash(password, password_hash_hash);
	} else {
		nt_password_hash(password, password_len, password_hash);
		hash_nt_password_hash(password_hash, password_hash_hash);
	}
	get_master_key(password_hash_hash, nt_response, master_key);
	wpa_hexdump_key(MSG_DEBUG, "MSCHAPV2: Master Key",
			master_key, MSCHAPV2_MASTER_KEY_LEN);
}


int mschapv2_verify_auth_response(const u8 *auth_response,
				  const u8 *buf, size_t buf_len)
{
	u8 recv_response[MSCHAPV2_AUTH_RESPONSE_LEN];
	if (buf_len < 2 + 2 * MSCHAPV2_AUTH_RESPONSE_LEN ||
	    buf[0] != 'S' || buf[1] != '=' ||
	    hexstr2bin((char *) (buf + 2), recv_response,
		       MSCHAPV2_AUTH_RESPONSE_LEN) ||
	    os_memcmp(auth_response, recv_response,
		      MSCHAPV2_AUTH_RESPONSE_LEN) != 0)
		return -1;
	return 0;
}
