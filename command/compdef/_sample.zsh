#compdef sample

function _sample() {
    _arguments -s : \
        {-e,--encode}'[encode n_cipher (default)]' \
        {-d,--decode}'[decode n_cipher]' \
        {-s,--seed}'[specify seed string]':keyword \
        {-m,--delimiter}'[specify delimiter string]':keyword \
        {-h,--help}'[display help and exit]' \
        {-v,--version}'[output version information and exit]' \
        '*:files:_files'
}

_sample
