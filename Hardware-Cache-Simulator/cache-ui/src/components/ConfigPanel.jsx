export default function ConfigPanel({ config, setConfig }) {

  function change(e) {
    setConfig({
      ...config,
      [e.target.name]: Number(e.target.value)
    });
  }

  return (
    <div className="config-panel">

      <h2>⚙ Cache Configuration</h2>

      <div className="config-grid">

        <div>
          <label>L1 Sets</label>
          <input
            type="number"
            name="L1s"
            value={config.L1s}
            onChange={change}
          />
        </div>

        <div>
          <label>L1 Associativity</label>
          <input
            type="number"
            name="L1E"
            value={config.L1E}
            onChange={change}
          />
        </div>

        <div>
          <label>L1 Delay</label>
          <input
            type="number"
            name="L1Delay"
            value={config.L1Delay}
            onChange={change}
          />
        </div>

        <div>
          <label>L2 Sets</label>
          <input
            type="number"
            name="L2s"
            value={config.L2s}
            onChange={change}
          />
        </div>

        <div>
          <label>L2 Associativity</label>
          <input
            type="number"
            name="L2E"
            value={config.L2E}
            onChange={change}
          />
        </div>

        <div>
          <label>L2 Delay</label>
          <input
            type="number"
            name="L2Delay"
            value={config.L2Delay}
            onChange={change}
          />
        </div>

      </div>

    </div>
  );
}