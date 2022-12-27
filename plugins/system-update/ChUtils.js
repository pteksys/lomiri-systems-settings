function parseChannel(channel) {
    /*
     * FIXME: this code relies on channels being named in a certain way.
     * Suported format:
     *  - 20.04/arm64/android9plus/devel
     *  - ubports-touch/16.04/rc
     */

    const seiresRegexp = /^[0-9]{2}\.[0-9]{2}$/; /* e.g. 16.04, 20.04 */
    const prettyStabilities = {
        "stable": i18n.tr("Stable"),
        "rc": i18n.tr("Release candidate"),
        "devel": i18n.tr("Development"),
    };

    let c = channel.split('/');
    let series;
    if (seiresRegexp.test(c[0]))
        series = c[0];
    else if (c[1] && seiresRegexp.test(c[1]))
        series = c[1];
    else
        series = '16.04';

    let stability = c[c.length - 1];

    let prettyStability = stability;
    if (prettyStabilities[stability])
        prettyStability = prettyStabilities[stability];

    return {
        series,
        stability,
        prettyName: `${series} ${prettyStability}`,
    };
}
