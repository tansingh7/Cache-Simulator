export default function StatsCards({ stats }) {

    return (

        <div className="cards">

            <div className="card">
                <h3>Hits</h3>
                <h1>{stats.hits}</h1>
            </div>

            <div className="card">
                <h3>Misses</h3>
                <h1>{stats.misses}</h1>
            </div>

            <div className="card">
                <h3>Hit Rate</h3>
                <h1>{stats.hitRate}%</h1>
            </div>

            <div className="card">
                <h3>Cycles</h3>
                <h1>{stats.totalCycles}</h1>
            </div>

        </div>

    );

}