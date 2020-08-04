package de.pschiessle.nfcserver

import android.nfc.NfcAdapter
import android.nfc.Tag
import android.nfc.tech.IsoDep
import android.os.Bundle
import android.view.Menu
import android.view.MenuItem
import androidx.appcompat.app.AppCompatActivity
import com.google.android.material.floatingactionbutton.FloatingActionButton
import com.google.android.material.snackbar.Snackbar
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity(), NfcAdapter.ReaderCallback {

    private var nfcAdapter: NfcAdapter? = null;

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        nfcAdapter = NfcAdapter.getDefaultAdapter(this);
        setContentView(R.layout.activity_main)

        findViewById<FloatingActionButton>(R.id.fab).setOnClickListener { view ->
            SuperAwesomePaymentSystem.doAwaitCard();
            Snackbar.make(view, "Awaiting card", Snackbar.LENGTH_LONG)
                .setAction("Action", null).show()
        }

        val adapter = GoodAdapter( findViewById(R.id.title), SuperAwesomePaymentSystem.availableGoods, this);
        good_list_view.adapter = adapter;
    }


    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        // Inflate the menu; this adds items to the action bar if it is present.
        menuInflater.inflate(R.menu.menu_main, menu)
        return true
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        return when (item.itemId) {
            R.id.action_settings -> true
            else -> super.onOptionsItemSelected(item)
        }
    }

    override fun onResume() {
        super.onResume()
        nfcAdapter?.enableReaderMode(
            this, this,
            NfcAdapter.FLAG_READER_NFC_A or
                    NfcAdapter.FLAG_READER_SKIP_NDEF_CHECK,
            null
        )
    }

    public override fun onPause() {
        super.onPause()
        nfcAdapter?.disableReaderMode(this)
    }

    override fun onTagDiscovered(tag: Tag?) {
        val isoDep = IsoDep.get(tag)
        isoDep.connect()
        //this data is an APDU command data string with is later divided into parts shown here
        // https://medium.com/the-almanac/how-to-build-a-simple-smart-card-emulator-reader-for-android-7975fae4040f
        val response = isoDep.transceive(
            Utils.hexStringToByteArray(
                "00A4040007A0000002471001"
            )
        )
        runOnUiThread {
            textView.text = ("\nCard Response: "
                    + Utils.toHex(response))
            if (SuperAwesomePaymentSystem.confirmPayment(Utils.toHex(response))) {
                Snackbar.make(findViewById(R.id.fab), "Pay successfull", Snackbar.LENGTH_LONG)
                    .setAction("Action", null).show()
            } else {
                Snackbar.make(findViewById(R.id.fab), "Pay failed", Snackbar.LENGTH_LONG)
                    .setAction("Action", null).show()
            }
        }
        isoDep.close()
    }
}