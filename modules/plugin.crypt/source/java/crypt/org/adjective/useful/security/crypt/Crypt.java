package org.adjective.useful.security.crypt;

import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;

import javax.crypto.Cipher;
import javax.crypto.KeyGenerator;
import javax.crypto.SecretKey;
import javax.crypto.spec.SecretKeySpec;

import android.util.Base64;

public class Crypt {
	public String crypt(String password, String salt) {
		String ret = null;
        ret = encrypt(password, salt);
		return ret;
	}

	private byte[] getKey(String salt) {
		byte[] seed = salt.getBytes();
		KeyGenerator kg;
		try {
			kg = KeyGenerator.getInstance("AES");
		} catch (NoSuchAlgorithmException e) {
			return null;
		}
		SecureRandom sr;
		try {
			sr = SecureRandom.getInstance("SHA1PRNG");
		} catch (NoSuchAlgorithmException e) {
			return null;
		}
		sr.setSeed(seed);
		kg.init(128, sr);
		SecretKey sk = kg.generateKey();
		byte[] key = sk.getEncoded();
		return key;
	}

	private String encrypt(String clearText, String salt) {
		byte[] encryptedText = null;
		try {
			SecretKeySpec ks = new SecretKeySpec(getKey(salt), "AES");
			Cipher c = Cipher.getInstance("AES");
			c.init(Cipher.ENCRYPT_MODE, ks);
			encryptedText = c.doFinal(clearText.getBytes("UTF-8"));
			return Base64.encodeToString(encryptedText, Base64.DEFAULT);
		} catch (Exception e) {
			return null;
		}

	}

	private String decrypt(String encryptedText, String salt) {

		byte[] clearText = null;
		try {
			SecretKeySpec ks = new SecretKeySpec(getKey(salt), "AES");
			Cipher c = Cipher.getInstance("AES");
			c.init(Cipher.DECRYPT_MODE, ks);
			clearText = c.doFinal(Base64.decode(encryptedText, Base64.DEFAULT));
			return new String(clearText, "UTF-8");
		} catch (Exception e) {
			return null;
		}
	}
}
