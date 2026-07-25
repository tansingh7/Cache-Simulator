function isActiveLine(cache, setIndex, lineIndex, line) {
  if (cache?.active) {
    return cache.active.set === setIndex && cache.active.way === lineIndex;
  }

  return Boolean(line?.active);
}

function renderCache(title, cache) {
  if (!cache || !Array.isArray(cache.cache)) {
    return (
      <div className="cache-box">
        <h3>{title}</h3>
        <p>No cache state available.</p>
      </div>
    );
  }

  return (
    <div className="cache-box">
      <h3>
        {title} ({cache.sets} sets x {cache.ways} ways)
      </h3>

      {cache.cache.map((set, setIndex) => (
        <div className="cache-set" key={setIndex}>
          <div className="set-title">Set {setIndex}</div>

          {set.map((line, lineIndex) => {
            const active = isActiveLine(cache, setIndex, lineIndex, line);

            return (
              <div
                key={lineIndex}
                className={active ? "cache-line active-line" : "cache-line"}
              >
                <div>Line {lineIndex}</div>
                <small>Tag: {line.tag}</small>
                <small>Valid: {line.valid ? "1" : "0"}</small>
                <small>Dirty: {line.dirty ? "1" : "0"}</small>
              </div>
            );
          })}
        </div>
      ))}
    </div>
  );
}

function CacheVisualizer({ current }) {
  return (
    <div className="cache-visualization">
      <h2>Cache Visualization</h2>

      {!current ? (
        <p>Select or play an access to view cache state.</p>
      ) : (
        <div className="cache-grid">
          {renderCache("L1 Cache", current.l1)}
          {renderCache("L2 Cache", current.l2)}
        </div>
      )}
    </div>
  );
}

export default CacheVisualizer;
