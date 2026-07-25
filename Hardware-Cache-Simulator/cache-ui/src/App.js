import { useState } from "react";
import axios from "axios";

import "./App.css";

import Navbar from "./components/Navbar";
import StatsCards from "./components/StatsCards";
import SearchBar from "./components/SearchBar";
import AccessTable from "./components/AccessTable";
import Charts from "./components/Charts";
import ConfigPanel from "./components/ConfigPanel";
import AccessPlayer from "./components/AccessPlayer";
import CacheVisualizer from "./components/CacheVisualizer";

function App() {
  const [data, setData] = useState(null);
  const [search, setSearch] = useState("");
  const [page, setPage] = useState(1);
  const [currentAccess, setCurrentAccess] = useState(null);

  const [config, setConfig] = useState({
    L1s: 2,
    L1E: 2,
    L1Delay: 7,
    L2s: 3,
    L2E: 5,
    L2Delay: 30,
  });

  const ROWS_PER_PAGE = 50;

  async function runSimulation() {
    try {
      const res = await axios.post(
        "http://localhost:5001/run",
        config
      );

      setData(res.data);
      setPage(1);
      setCurrentAccess(null);
    } catch (err) {
      console.error(err);
      alert("Simulation failed");
    }
  }

  const filtered = data
    ? data.accesses.filter((a) =>
        a.address.toLowerCase().includes(search.toLowerCase())
      )
    : [];

  const totalPages =
    filtered.length === 0
      ? 1
      : Math.ceil(filtered.length / ROWS_PER_PAGE);

  const paginated = filtered.slice(
    (page - 1) * ROWS_PER_PAGE,
    page * ROWS_PER_PAGE
  );

  return (
    <div>
      <Navbar />

      <div className="container">

        <ConfigPanel
          config={config}
          setConfig={setConfig}
        />

        <button onClick={runSimulation}>
          ▶ Run Simulation
        </button>

        {data && (
          <>
            <StatsCards stats={data.statistics} />

            <Charts data={data} />

            <AccessPlayer
              accesses={data.accesses}
              current={currentAccess}
              setCurrent={setCurrentAccess}
            />

            <CacheVisualizer current={currentAccess} />
            
            <SearchBar
              value={search}
              setValue={(value) => {
                setSearch(value);
                setPage(1);
              }}
            />

            <AccessTable accesses={paginated} />

            <div className="pagination">
              <button
                disabled={page === 1}
                onClick={() => setPage(page - 1)}
              >
                ◀ Previous
              </button>

              <span>
                Page {page} of {totalPages}
              </span>

              <button
                disabled={page === totalPages}
                onClick={() => setPage(page + 1)}
              >
                Next ▶
              </button>
            </div>
          </>
        )}
      </div>
    </div>
  );
}

export default App;
