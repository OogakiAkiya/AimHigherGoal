class OpenSSLRSA {
public:
	OpenSSLRSA();
	~OpenSSLRSA();

	void CreateKey(const std::string _private_key_file,const std::string _public_key_file);
private:
	int size = 1024;
	unsigned long expornent = 65537;
	RSA* privateKey;

	std::string key = "-----BEGIN RSA PRIVATE KEY-----\n"
		"MIICXAIBAAKBgQCifBd8eFizzyTnsiuetqqEIGzu1NnkMI / nqRiu + / ObM5Nh79Ej\n"
		"wiPUsSlBD + CRaycntJZSnawMLiu2cTlm5MR2nqwr26H9iBIT3Y1bOBz2rpdc / EeB\n"
		"bvvxfJZKobxs2Sbq8Qppnvwdmt5PCvI5Cj4m7yhnRNdJjRSy8osb0s6FJwIDAQAB\n"
		"AoGAMX5xPQX / pw3zJOyd4uOrf0HNTNbjZGHyg9N7ZFn085K7UzrUzvqlJXWtplbq\n"
		"xgEI1Uxp + dJzhOZPhGxq2s / Wk / r5v4B / Qxk + tIb20Pv3hDcD1Xx3Q84cl + 75vZQ1\n"
		"qhzj7jQWzlkzrB9ALtbmUkHU8eWo6V6jwJo00wnAbMv4hnECQQDYjkvIm4SvUv4W\n"
		"HqPu + iOz + mxgB9L5VnHyuXvoCedHPqyON / LUWJ6DBKIc6mNJcmTXTvOBGNmz4ulm\n"
		"HwKV7gz / AkEAwBSIkyz / 7N6OsrsH1ZM1e + a1VdL1XrMhFaHp82anCGKRaBRpLDOH\n"
		"Q0sMhEwERo6 / EmOTfwuvFS2q8Zhy5W1 / 2QJAfSNlOYxc1S + BMmkMVDlpzkQEwk1j\n"
		"JcBFYny61DB83FekQR6ljP2Uw6M2xrzPGQ8opJ6z6cLhcqjKNL / wr5D7IwJADkF3\n"
		"4XR1ih76eElpS3Chh5CX5MbDb0PcML7Tnj0AKJ8oK5FYPO + WGmh3hVoe9x2hELxT\n"
		"Nhqn4 + p / xW6LGHwIAQJBAMLLAvciSM1RXasdK / T + gMMx63pL7tHvkUMsD1AFcDvR\n"
		"NqWnGCRBhyhjtkb3IEi + VG30L04AspGFMKdwd0LV / Ww =\n"
		"-----END RSA PRIVATE KEY-----\n";

	std::string pubKey= "-----BEGIN PUBLIC KEY-----\n"
		"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCifBd8eFizzyTnsiuetqqEIGzu\n"
		"1NnkMI/nqRiu+/ObM5Nh79EjwiPUsSlBD+CRaycntJZSnawMLiu2cTlm5MR2nqwr\n"
		"26H9iBIT3Y1bOBz2rpdc/EeBbvvxfJZKobxs2Sbq8Qppnvwdmt5PCvI5Cj4m7yhn\n"
		"RNdJjRSy8osb0s6FJwIDAQAB\n"
		"-----END PUBLIC KEY-----\n";

};