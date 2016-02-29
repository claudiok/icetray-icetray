
from icecube.icetray import memory

def print_snapshot(snap):
    """
    Prints an :class:`icecube.icetray.memory.Snapshot` object

    Args:
        snap (Snapshot): The snapshot object.
    """
    print('{:<40} | {:>10}'.format('Label','Size'))
    print('{:-<53}'.format(''))
    total = 0
    for k,v in snap.items():
        total += v
        print('{:<40} | {:>10d}'.format(k,v))
    print('{:-<53}'.format(''))
    print('{:<40} | {:>10d}'.format('Total',total))


def graph_timeline(timeline, filename, limit=10):
    """
    Graph a MemoryTimeline object

    Args:
        timeline (MemoryTimeline): The :class:`icecube.icetray.memory.MemoryTimeline` object.
        filename (str): A filename to write to.
        limit (int): Number of lines to display (from highest to lowest).
    """
    import matplotlib
    matplotlib.use('Agg')
    import matplotlib.pyplot as plt

    fig = plt.figure()
    ax = fig.add_subplot(111)
    ax.set_ylabel('Memory (KB)')
    ax.set_xlabel('Time (s)')

    series = {}
    for ts in timeline:
        t = float(ts.first)
        for k,v in ts.second.items():
            if k not in series:
                series[k] = {'t':[],'v':[]}
            series[k]['t'].append(t*1.0/1000000)
            series[k]['v'].append(v*1.0/1000)
    highest_series = sorted(series,key=lambda k:max(series[k]['v']),reverse=True)[:10]
    
    max_mem = max(series[highest_series[0]]['v'])
    ax.set_ylim([0,int(max_mem*1.1)])

    for k in highest_series:
        ax.plot(series[k]['t'],series[k]['v'],label=k)

    legend = ax.legend(loc='upper left')

    plt.savefig(filename, dpi=300)

try:
    import tracemalloc
    import linecache
except ImportError:
    pass
else:
    def tracemalloc_display_highest(snapshot, group_by='lineno', limit=10):
        snapshot = snapshot.filter_traces((
            tracemalloc.Filter(False, "<frozen importlib._bootstrap>"),
            tracemalloc.Filter(False, "<frozen importlib._bootstrap_external>"),
            tracemalloc.Filter(False, "<unknown>"),
        ))
        top_stats = snapshot.statistics(group_by)

        print("Top %s lines" % limit)
        for index, stat in enumerate(top_stats[:limit], 1):
            frame = stat.traceback[0]
            # replace "/path/to/module/file.py" with "module/file.py"
            filename = os.sep.join(frame.filename.split(os.sep)[-2:])
            print("#%s: %s:%s: %.1f KiB"
                  % (index, filename, frame.lineno, stat.size / 1024))
            line = linecache.getline(frame.filename, frame.lineno).strip()
            if line:
                print('    %s' % line)

        other = top_stats[limit:]
        if other:
            size = sum(stat.size for stat in other)
            print("%s other: %.1f KiB" % (len(other), size / 1024))
        total = sum(stat.size for stat in top_stats)
        print("Total allocated size: %.1f KiB" % (total / 1024))
